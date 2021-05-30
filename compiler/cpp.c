#include <ctype.h>
#include <libgen.h>
#include <locale.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include "compiler.h"

static Map *macros = &EMPTY_MAP;
static Map *once = &EMPTY_MAP;
static Map  *keywords = &EMPTY_MAP;
static Map *include_guard = &EMPTY_MAP;
static Vector *cond_incl_stack = &EMPTY_VECTOR;
static Vector *std_include_path = &EMPTY_VECTOR;
static struct tm now;
static Token *cpp_token_zero = &(Token){ .kind=TNUMBER, .sval = "0" };
static Token *cpp_token_one = &(Token){.kind=TNUMBER, .sval="1"};

typedef void SpecialMacroHandler(Token *tok);
typedef enum {IN_THEN, IN_ELF, IN_ELSE} CondInclCtx;
typedef enum {  MACRO_OBJ, MACRO_FUNC, MACRO_SPECIAL } MacroType;

typedef struct {
    CondInclCtx ctx;
    char *include_guard;
    File *file;
    bool wastrue;   
} CondIncl;

typedef struct {
    MacroType kind;
    int nargs;
    Vector *body;
    bool is_varg;
    SpecialMacroHandler *fn;
} Macro;

static Macro *make_obj_macro(Vector *body);
static Macro *make_func_macro(Vector *body, int nargs, bool is_varg);
static Macro *make_special_macro(SpecialMacroHandler *fn);
static void define_obj_macro(char *name, Token *value);
static void read_directive(Token *hash);
static Token *read_expand(void);

// constructor
static CondIncl * make_cond_incl(bool wastrue){
    CondIncl *r = calloc(1, sizeof(CondIncl));
    r->ctx = IN_THEN;
    r->wastrue = wastrue;
    return r;
}

static Macro *make_macro(Macro *tmpl){
    Macro *r = malloc(sizeof(Macro));
    *r = *tmpl;
    return r;
}

static Macro *make_obj_macro(Vector *body){
    return make_macro(&(Macro){MACRO_OBJ, .body = body});
}

static Macro *make_func_macro(Vector *body, int nargs, bool is_varg){
    return make_macro(&(Macro) { MACRO_FUNC, .nargs = nargs, .body=body, .is_varg=is_varg});
}

static Macro *make_special_macro(SpecialMacroHandler *fn){
    return make_macro(&(Macro){MACRO_SPECIAL, .fn=fn});
}

static Token *make_macro_token(int position, bool is_vararg){
    Token *r = malloc(sizeof(Token));
    r->kind = TMACRO_PARAM;
    r->is_vararg = is_vararg;
    r->hideset = NULL;
    r->position = position;
    r->space = false;
    r->bol = false;
    return r;
}

static Token *copy_token(Token *tok){
    Token *r = malloc(sizeof(Token));
    *r = *tok;
    return r;
}

static void expect(char id){
    Token *tok = lex();
    if (!is_keyword(tok, id)){
       errort(tok, "%c expected , but got %s", id, tok2s(tok)) ;
    }
}

bool is_ident(Token *tok, char *s){
    return tok->kind == TIDENT && !strcmp(tok->sval, s);
}

static bool next(int id){
    Token *tok = lex();
    if (is_keyword(tok, id)){
        return true;
    }
    unget_token(tok);
    return false;
}

static void propagate_space(Vector *tokens, Token *tmpl){
    if (vec_len(tokens) == 0){
       return;    
    }
    Token *tok = copy_token(vec_head(tokens));
    tok->space = tmpl->space;
    vec_set(tokens,0, tok);
}

// macro expanders

static Token *read_ident(){
    Token *tok = lex();
    if (tok->kind != TIDENT){
        errort(tok, "identifier expected, but got %s", tok2s(tok));
    }
    return tok;
}

void expect_newline(){
    Token *tok = lex();
    if (tok->kind != TNEWLINE){
        errort(tok, "newline expected, but got %s", tok2s(tok));
    }
}

static Vector *read_one_arg(Token *ident, bool *end,bool readall){
    Vector *r = make_vector();
    int level = 0;
    for (;;){
        Token *tok = lex();
        if (tok->kind == TEOF){
            errort(ident, "unterminated macro argument list");
        }
        if (tok->kind == TNEWLINE){
            continue;
        }
        if (tok->bol && is_keyword(tok, "#")){
            read_directive(tok);
            continue;
        }
        if (level == 0 && is_keyword(tok, ')')){
            unget_token(tok);
            *end = true;
            return r;
        }
        if (level == 0 && is_keyword(tok, ',') && !readall){
            return r;
        }
        if (is_keyword(tok, '(')){
            level++;
        }
        if (is_keyword(tok, ')')){
            level--;
        }

        if (tok->bol){
            tok=copy_token(tok);
            tok->bol = false;
            tok->space = true;
        }
        vec_push(r, tok);
    }
}

static Vector *do_read_args(Token *ident, Macro *macro){
    Vector *r = make_vector();
    bool end = false;
    while (!end){
        bool in_ellipsis = (macro->is_varg && vec_len(r) + 1 == macro->nargs);
        vec_push(r, read_one_arg(ident, &end, in_ellipsis));
    }
    if (macro->is_varg && vec_len(r) == macro->nargs -1 ){
        vec_push(r, make_vector());
    }
    return r;
}

static Vector *read_args(Token *tok, Macro *macro){
    if (macro->nargs == 0 && is_keyword(peek_token(), ')')){
        // if a macro M has no parameter , argument list of M()
        // is an empty list. If it has one parameter, argument list M() is a list containing an empty list
        return make_vector(); 
    }
    Vector *args = do_read_args(tok, macro);
    if (vec_len(args) != macro->nargs){
        errort(tok , "macro argument number does not match ");
    }
    return args;
}

static Vector *add_hide_set(Vector *tokens, Set *hideset){
    Vector *r = make_vector();
    for (int i=0; i<vec_len(tokens); i++){
        Token *t = copy_token(vec_get(tokens, i));
        t->hideset = set_union(t->hideset, hideset);
        vec_push(r, t);
    }
    return r;
}

static Token *glue_tokens(Token *t, Token *u){
    Buffer *b = make_buffer();
    buf_printf(b, "%s", tok2s(t));
    buf_printf(b, "%s", tok2s(u));
    Token *r = lex_string(buf_body(b));
    return r;
}

static void glue_push(Vector *tokens, Token *tok){
    Token *last = vec_pop(tokens);
    vec_push(tokens, glue_tokens(last, tok));
}

static Token *stringize(Token *tmpl, Vector *args){
    Buffer *b= make_buffer();
    for (int i=0; i<vec_len(args); i++){
        Token *tok = vec_get(args,i);
        if (buf_len(b) && tok->space ){
            buf_printf(b, " ");
        }
        buf_printf(b, "%s", tok2s(tok));
    }
    buf_write(b, '\0');
    Token *r = copy_token(tmpl);
    r->kind = TSTRING;
    r->sval = buf_body(b);
    r->slen = buf_len(b);
    r->enc = ENC_NONE;
    return r;
}

static Vector *expand_all(Vector *tokens, Token *tmpl){
    token_buffer_stash_reverse(tokens);
    Vector *r = make_vector();
    for (;;){
        Token *tok = read_expand();
        if (tok->kind == TEOF){
            break;
        }
        vec_push(r, tok);
    }
    propagate_space(r, tmpl);
    token_buffer_unstash();
    return r;
}

static Vector *subst(Macro *macro, Vector *args, Set *hideset){
    Vector *r = make_vector();
    int len = vec_len(macro->body);
    for (int i=0;i<len; i++){
        Token *t0 = vec_get(macro->body, i);
        Token *t1 = (i == len -1) ? NULL : vec_get(macro->body, i+1);
        bool t0_param = (t0->kind == TMACRO_PARAM);
        bool t1_param = (t1 && t1->kind == TMACRO_PARAM);

        if (is_keyword(t0, '#') && t1_param){
            vec_push(r, stringize(t0, vec_get(args, t1->position)));
        
        i++;
        continue;
        }
        if (is_keyword(t0, KHASHHASH) && t1_param){
          Vector *arg = vec_get(args, t1->position);
          if (t1->is_vararg && vec_len(r) > 0 && is_keyword(vec_tail(r), ',')){
              if (vec_len(arg) > 0){
                  vec_append(r, arg);
              }
              else {
                  vec_pop(r);
              }
          }
          else if (vec_len(arg) > 0){
             glue_push(r, vec_head(arg));
             for (int i=1; i<vec_len(arg);i++){
                 vec_push(r, vec_get(arg, i));
             }
          }
          i++;
          continue;
        }
        if (is_keyword(t0, KHASHHASH) && t1){
            hideset = t1->hideset;
            glue_push(r, t1);
            i++;
            continue;
        }
        if (t0_param && t1 && is_keyword(t1, KHASHHASH)){
            hideset = t1->hideset;
            Vector *arg = vec_get(args, t0->position);
            if (vec_len(arg) ==0 ){
                i++;
            }
            else {
                vec_append(r, arg);
            }
            continue;
        }
        if (t0_param){
            Vector *arg = vec_get(args, t0->position);
            vec_append(r, expand_all(arg, t0));
            continue;
        }
        vec_push(r, t0);

    }
    return add_hide_set(r, hideset);
}

// error missing positions
static void unget_all(Vector *tokens){
    for (int i=vec_get(tokens) -1; i>= 0; i--){
        unget_token(vec_get(tokens, i));
    }
}

// epxand function to Dave Prosser's docs

static Token *read_expand_newline(){
    Token *tok = lex();
    if (tok->kind != TIDENT){
        return tok;
    }
    char *name = tok->sval;
    Macro *macro = map_get(macros, name);
    if (!macro || set_has(tok->hideset, name)){
        return tok;
    }
    switch (macro->kind)
    {
    case MACRO_OBJ:{
        Set *hideset = set_add(tok->hideset,name) ;
        Vector *tokens = subst(macro, NULL, hideset);
        propagate_space(tokens, tok);
        unget_all(tokens);
        return read_expand();
    }
    case MACRO_FUNC: {
        if (!next('(')){
            return tok;
        }
        Vector *args = read_args(tok, macro);
        Token *rparam = peek_token();
        expect(')');
        Set *hideset = set_add(set_intersection(tok->hideset, rparam->hideset), name);
        Vector *tokens = subst(macro, args, hideset);
        propagate_space(tokens, tok);
        unget_all(tokens);
        return read_expand();   
    }
    case MACRO_SPECIAL:
        macro->fn(tok);
        return read_expand();
    default:
        error("internal error");
    }
}

static Token *read_expand(){
    for (;;){
        Token *tok = read_expand_newline();
        if (tok->kind != TNEWLINE){
            return tok;
        }
        }
}

static bool read_funclike_macro_params(Token *name , Map *param){
    int pos = 0;
    for (;;){
        Token *tok = lex();
        if (is_keyword(tok, ')')){
          return false;
        }
        if (pos){
            if (!is_keyword(tok, ',')){
              errort(tok, ", expected, but got %s", tok2s(tok));
            }
        tok = lex();
      }
      if (tok->kind == TNEWLINE){
          errort(name, "missing ')' in macro parameter list ");
      }
      if (is_keyword(tok, KELLIPSIS)){
          map_put(param, "__VA_ARGS__", make_macro_token(pos++, true));
          expect(')');
          return true;
      }
      if (tok->kind != TIDENT){
          errort(tok, "identifier expected, but got %s", tok2s(tok));
      }
      char *arg  =tok->sval;
      if (next(KELLIPSIS)){
          expect(')');
          map_put(param, arg, make_macro_token(pos++, true));
          return true;
      }
    map_put(param , arg, make_macro_token(pos++, false)); 
    }
}


static void hashmap_check(Vector *v){
    if (vec_len(v) == 0){
        return;
    }
    if (is_keyword(vec_head(v) ,KHASHHASH)){
        errort(vec_head(v), "'##' cannnot appear at start of macro-expansion");
    }
    if (is_keyword(vec_tail(v), KHASHHASH)){
        errort(vec_tail(v), "'##' cannot appear at end of macro expansion");
    }
}


static Vector *read_funclike_macro_body(Map *param){
    Vector *r = make_vector();
    for (;;){
        Token *tok = lex();
        if (tok->kind == TNEWLINE){
            return;
        }
        if (tok->kind == TIDENT){
            Token *subst = map_get(param, tok->sval);
            if (subst){
                subst = copy_token(subst);
                subst->space = tok->space;
                vec_push(r, subst);
                continue;
            }
        }
        vec_push(r, tok);
    }
}

static void read_funclike_macro(Token *name){
    Map *param = make_map();
    bool is_varg = read_funclike_macro_params(name, param);
    Vector *body = read_funclike_macro_body(param);
    hashmap_check(body);
    Macro *macro = make_func_macro(body, map_len(param), is_varg);
    map_put(macros, name->sval, macro);
}

static void read_obj_macro(char *name){
    Vector *body = make_vector();
    for (;;){
        Token *tok = lex();
        if (tok->kind == TNEWLINE){
            break;
        }
        vec_push(body, tok);
    }
    hashmap_check(body);
    map_put(macros, name, make_obj_macro(body));
}

/*
 *#define
*/
static void read_define(){
    Token *name = read_ident();
    Token *tok = lex();
    if (is_keyword(tok, '(') && !tok->space){
        read_funclike_macro(name);
        return;
    }
    unget_token(tok);
    read_obj_macro(name->sval);
}

// #undef

static void read_undef(){
    Token *name = read_ident();
    expect_newline();
    map_remove(macros, name->sval);
}

// #if and the line

static Token *read_defined_op(){
    Token *tok = lex();
    if (is_keyword(tok, "(")){
        tok= lex();
        expect(')');
    }
    if (tok->kind != TIDENT ){
        errort(tok, "identifier expected, but got %s", tok2s(tok));
    }
    return map_get(macros, tok->sval) ? cpp_token_one: cpp_token_zero;
}

static Vector *read_intexpr_line(){
    Vector *r = make_vector();
    for (;;){
        Token *tok = read_expand_newline();
        if (tok->kind == TNEWLINE){
            return r;
        }
        if (is_ident(tok, "defined")){
            vec_push(r, read_defined_op());
        }
        else if (tok->kind == TIDENT){
            vec_push(r , cpp_token_zero);
        }
        else {
            vec_push(r, tok);
        }
    }
}

static bool read_constexpr(){
    token_buffer_stash(vec_reverse(read_intexpr_line()));
    Node *expr = read_expr();
    Token *tok = lex();
    if (tok->kind != TEOF){
        errort(tok, "stray token : %s", tok2s(tok));
    }
    token_buffer_unstash();
    return eval_intexpr(expr, NULL);
}

static void do_read_if(bool istrue){
    vec_push(cond_incl_stack, make_cond_incl(istrue));
    if (!istrue){
        skip_cond_incl();
    }
}

static void read_if(){
    do_read_if(read_constexpr());
}

static void read_ifdef(){
    Token *tok = lex();
    if (tok->kind !=TIDENT){
        errort(tok, "identifier expected, but got %s", tok2s(tok));
    }
    expect_newline();
    do_read_if(map_get(macros, tok->sval));
}

static void read_ifndef(){
    Token *tok = lex();
    if (tok->kind != TIDENT){
        errort(tok,"identifier expected , but got %s", tok2s(tok));
    }
    expect_newline();
    do_read_if(!map_get(macros, tok->sval));
    if (tok->count == 2){
        CondIncl *cl = vec_tail(cond_incl_stack);
        cl->include_guard = tok->sval;
        cl->file = tok->file;
    }
}

static void read_else(Token *hash){
    if (vec_len(cond_incl_stack) == 0){
        errort(hash, "stray #else");
    }
    CondIncl *cl  = vec_tail(cond_incl_stack);
    if (cl->ctx == IN_ELSE){
        errort(hash, "#elif after #else");
    }
    cl->ctx = IN_ELF;
    cl->include_guard =  NULL;
    if (cl->wastrue || !read_constexpr()){
        skip_cond_incl();
        return;
    }
    cl->wastrue = true;
}

// after skipping newlines , return non-newline token.
static Token *skip_newlines(){
    Token *tok = lex();
    while (tok->kind == TNEWLINE){
        tok = lex();
    }
    unget_token(tok);
    return tok;
}

static void read_endif(Token *hash){
    if (vec_len(cond_incl_stack)==0){
        errort(hash, "stray #endif");
    }
    CondIncl *cl = vec_pop(cond_incl_stack);
    expect_newline();
    /*
     * Detect an #ifndef and #endif pair that guards the entire header file. remember the macro name guarding the file so that we can skip the file next time.
     */
    if (!cl->include_guard || cl->file != hash->file){
        return;
    }
    Token *last = skip_newlines();
    if (cl->file != last->file){
        map_put(include_guard, cl->file->name, cl->include_guard);
    }
}

/*
#error and #warnings
*/ 
static char *read_error_message(){
    Buffer *b = make_buffer();
    for (;;){
        Token *tok = lex();
        if (tok->kind == TNEWLINE){
            return buf_body(b);
        }
        if (buf_len(b) != 0 &&tok->space){
            buf_write(b, ' ');
        }
        buf_printf(b, '%s', tok2s(tok));
    }
}

static void read_error(Token *hash){
    errort(hash, "error : %s ", read_error_message());
}

static void read_warning(Token *hash){
    errort(hash, "warning : %s", read_error_message());
}

/*
#include
*/
static char *join_paths(Vector *args){
    Buffer *b = make_buffer();
    for (int i=0; i<vec_len(args); i++){
        buf_printf(b, "%s", tok2s(vec_get(args, i)));
    }
    return buf_body(b);
}

static char *read_cpp_header_name(Token *hash, bool *std){
    char *path = read_header_file_name(std);
    if (path){
        return path;
    }
    /*
    if a token following #include does not start with < nor " then try to read the token as a regular token. Macro-expanded form may be a valid header file path.
    */
    Token *tok = read_expand_newline();
    if (tok->kind == TNEWLINE){
        errort(hash, "expected filename, but got newline.");
    }
    if (tok->kind == TSTRING){
        *std = false;
        return tok->sval;
    }
    if (!is_keyword(tok, '<')){
        errort(tok, " < expected , but got %s", tok2s(tok));
    }
    Vector *tokens = make_vector();
    for (;;){
        Token *tok = read_expand_newline();
        if (tok->kind == TNEWLINE) {
            errort(hash, "premature end of header name");
        }   
        if (is_keyword(tok, ">")){
            break;
        }
        vec_push(tokens, tok);
    }
    *std = true;
    return join_paths(tokens);
}

static bool guarded(char *path){
    char *guard = map_get(include_guard, path);
    bool r = (guard && map_get(macros, guard));
    define_obj_macro("__compile_include_guard", r  ? cpp_token_one: cpp_token_zero);
    return r;
}

static bool try_include (char *dir, char *filename, bool isimport){
    char *path = fullpath(format("%s/%s", dir, filename));
    if (map_get(once, path)){
        return true;
    }
    if (guarded(path)){
        return true;
    }
    FILE *fp = fopen(path, "r");
    if (!fp){
      return false;
    }
    if (isimport){
        map_put(once, path, (void *)1);

    }
    stream_push(make_file(fp, path));
    return true;
}

static void read_include(Token *hash, File *file, bool isimport){
    bool std;
    char *filename = read_cpp_header_name(hash, &std);
    expect_newline();
    if (filename[0] == '/'){
        if (try_include("/" ,filename, isimport)){
            return;
        }
        goto err;
    }
    if (!std){
        char *dir  = file->name ? dirname(strdup(file->name)) : ".";
        if (try_include(dir, filename, isimport)){
            return;
        }
    }
    for (int i=0; i<vec_len(std_include_path); i++){
        if (try_include(vec_get(std_include_path, i), filename, isimport)){
            return;
        }
    }
    err:
       errort(hash, "cannot find header file : %s", filename);
}

static void read_include_next(Token *hash, File *file){
    /*
    #include_next include's the next file from the search path. 
    This feature is used to override a header file without getting into infinite inclusion loop.
    */
   bool std;
   char *f = read_cpp_header_name(hash , &std);
   expect_newline();
   if (f[0] == '/'){
       if (try_include('/', f, false)){
           return;
       }
       goto err;
   }
   char *cur = fullpath(file->name);
   int i = 0;
   for (; i <vec_len(std_include_path); i++){
      char *dir = vec_get(std_include_path, i)  ;
      if (strcmp(cur, fullpath(format("%s/%s", dir, f)))){
          break;
      }
   }
   for (i++; i<vec_len(std_include_path); i++){
       if (try_include(vec_get(std_include_path, i), f , false)){
           return;
       }
   } 
   err:
      errort(hash, "cannot find header file: %s", f);
}

/*
#pragma
*/
static void parse_pragma_operand(Token *tok){
    char *s = tok->sval;
    if (!strcmp(s, "once")){
        char *path = fullpath(tok->file->name);
        map_put(once, path, (void *)1);
    }
    else if (!strcmp(s, "enable_warning")){
        enable_warning = true;
    }
    else if (!strcmp(s, "disable_warning")){
        enable_warning = false;
    }
    else {
        errort(tok, "unknown #pragma: %s", s);
    }
}

static void read_pragma(){
    Token *tok = read_ident();
    parse_pragma_operand(tok);
}

/*
#line
*/
static bool is_digit_seq(char *c){
    for (; *c; c++){
        if (!isdigit(*c)){
            return false;
        }

    }
    return true;
}

static void read_line(){
    Token *tok = read_expand_newline();
    if (tok->kind != TNUMBER || !is_digit_seq(tok->sval)){
        errort(tok, "number expected after #line, but got %s",tok2s(tok));
    }
    int line = atoi(tok->sval);
    tok = read_expand_newline();
    char *f = NULL;
    if (tok->kind == TSTRING){
        f = tok->sval;
        expect_newline();
    }
    else if (tok->kind != TNEWLINE){
        errort(tok, "newline or a source name are expected, but got %s", tok2s(tok));
    }
    File *fp = current_file();
    fp->line = line;
    if (f){
        fp->name = f;
    }
}

static void read_linemarker(Token *tok){
    if (!is_digit_seq(tok->sval)){
        errort(tok , "line number expected, but got %s", tok2s(tok));
    }
    int line = atoi(tok->sval);
    tok = lex();
    if (tok->kind != TSTRING){
        errort(tok, "filename expected, but got %s", tok2s(tok));
    } 
    char *f = tok->sval;
    do {
        tok = lex();
    } 
    while(tok->kind != TNEWLINE);
    File *fp = current_file();
    fp->line = line;
    fp->name = f;
    
}

/*
#directive
*/
static void read_directive(Token *hash){
    Token *tok = lex();
    if (tok->kind == TNEWLINE){
       return;
    }
    if (tok->kind == TNUMBER){
        read_linemarker(tok);
        return;
    }
    if (tok->kind != TIDENT){
        goto err;
    }
    char *s = tok->sval;
    if (!strcmp(s, "define"))   {         read_define();
    }else if (!strcmp(s, "elif")) {        read_elif(hash);
    }else if (!strcmp(s, "else"))    {     read_else(hash);
}else if (!strcmp(s, "endif"))      {  read_endif(hash);
 }   else if (!strcmp(s, "error"))    {    read_error(hash);
  }  else if (!strcmp(s, "if"))        {   read_if();
   } else if (!strcmp(s, "ifdef"))    {    read_ifdef();
    }else if (!strcmp(s, "ifndef"))   {    read_ifndef();
    }else if (!strcmp(s, "import"))  {     read_include(hash, tok->file, true);
    }else if (!strcmp(s, "include")){
        read_include(hash, tok->file, false);
    }
    else if (!strcmp(s, "include_next")){ 
        read_include_next(hash, tok->file);
    }
    else if (!strcmp(s, "line")){
       read_line();
    }
    else if (!strcmp(s, "pragma")){
        read_pragma();
    }
    else if (!strcmp(s, "undef")) {     
      read_undef();
    }
    else if (!strcmp(s, "warning")){ 
        read_warning(hash);
    }
    else{
    goto err;
    }
    return;

    err:
        errort(hash,"unsupported preprocessor directive : %s", tok2s(tok));     

}


static void make_token_pushback(Token *tmpl, int kind,char *sval){
    Token *tok  = copy_token(tmpl);
    tok->kind = kind;
    tok->sval = sval;
    tok->slen = strlen(sval) + 1;
    tok->enc = ENC_NONE;
    unget_token(tok);
}

static void handle_date_macro(Token *tmpl){
    char buf[20];
    strftime(buf, sizeof(buf), "%b %e %Y", &now);
    make_token_pushback(tmpl, TSTRING, strdup(buf));
}

static void handle_time_macro(Token *tmpl){
    char buf[20];
    strftime(buf , sizeof(buf), "%T", &now);
    make_token_pushback(tmpl, TSTRING, strdup(buf));
}

static void handle_timestamp_macro(Token *tmpl){
    /*
    __TIMESTAMP__ is expanded to a string that describes the date and time of the last modification time of current source file
    */
   char buf[30];
   strftime(buf, sizeof(buf), "%a %b %e %T %Y", localtime(&tmpl->file->mtime));
   make_token_pushback(tmpl, TSTRING, strdup(buf));
}

/// macros










