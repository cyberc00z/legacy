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







