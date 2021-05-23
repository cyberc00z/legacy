/*
Recursive descendent parser for C
*/
#include <ctype.h>
#include <errno.h>
#include <limits.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include "compiler.h"

// The largest alignment requirement on x86-64. When we are allocating memory
// for an array whose type is unkown , the array will be aligned to this boundary
#define MAX_ALIGN 16

#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define MIN(a,b) ((a) < (b) ? (a) : (b))

// THe last source location we want to point to when we find an error in the source code.
SourceLoc *source_loc;

// Objects representing various scopes . Did you know C has so many different scopes.? You can use the same name for global variable, local variable
// struct/union/enum tag and goto label!
static Map *globalenv = &EMPTY_MAP;
static Map *localenv;
static Map *tags = &EMPTY_MAP;
static Map *labels;

static Vector *toplevels;
static Vector *localvars;
static Vector *gotos;
static Vector *cases;
static Type *current_func_type;

static char *defaultcase;
static char *lbreak;
static char *lcontinue;


// Object representing basic types . ALl varaibles will be one of these types.
// Or a dericed type from one of them. Note that (typename){initializer} is C99 feature to write struct literals

Type *type_void  = &(Type){ KIND_VOID, 0, 0, false};
Type *type_bool  = &(Type){ KIND_BOOL ,1,1, true };
Type *type_char = &(Type){ KIND_CHAR,1,1,false };
Type *type_short = &(Type){ KIND_SHORT ,2,2,false };
Type *type_int = &(Type){ KIND_INT,4,4,false  };
Type *type_long = &(Type){ KIND_LONG, 8,8,false  };
Type *type_llong = &(Type){ KIND_LLONG,8,8,false };
Type *type_uchar = &(Type){ KIND_CHAR,1,1,true };
Type *type_ushort = &(Type){ KIND_SHORT,2,2,true };
Type *type_uint = &(Type){ KIND_INT, 4,4, true  };
Type *type_ulong = &(Type){ KIND_LONG,8,8,true  };
Type *type_ullong = &(Type){ KIND_LLONG,8,8,true  };
Type *type_float = &(Type){ KIND_FLOAT, 4,4,false };
Type *type_double = &(Type){ KIND_DOUBLE, 8,8,false  };
Type *type_ldouble = &(Type){ KIND_LDOUBLE, 8, 8,false };
Type *type_enum = &(Type){ KIND_ENUM, 4,4,false  };

static Type* make_ptr_type(Type *ty);
static Type* make_array_type(Type *ty, int size);
static Node *read_compound_stmt(void);
static void read_decl_or_stmt(Vector *list);
static Node *conv(Node *node);
static Node *read_stmt(void);
static bool is_type(Token *tok);
static Node *read_unary_expr(void);
static void read_decl(Vector *toplevel, bool isglobal);
static Type *read_declarator_tail(Type *baseType,Vector *params );
static Type *read_declarator(char **name, Type *basetype, Vector *params);
static Type *read_abstract_declarator(Type *basetype);
static Type *read_decl_spec(int *sclass);
static Type *read_struct_field(Node *struc);
static void read_initializer_list(Vector *inits, Type *ty, int off, bool designated);
static Type *read_cast_type(void);
static Vector *read_decl_init(Type *ty);
static Node *read_boolean_expr(void);
static Node *read_conditional_expr(void);
struct Node *read_assigment_expr(void);
struct Node *read_cast_expr(void);
struct Node *read_comma_expr(void);
static Token *get(void);
static Token *peek(void);

typedef struct {
    int beg;
    int end;
    char *label;
} Case;

enum {
    S_TYPEDEF = 1,
    S_EXTERN,
    S_STATIC,
    S_AUTO,
    S_REGISTER,
};

enum {
    DECL_BODY = 1,
    DECL_PARAM,
    DECL_PARAM_TYPEONLY,
    DECL_CAST,
};

/*
Source Location
*/
static void mark_location(){
    Token *tok =peek();
    source_loc = malloc(sizeof(SourceLoc));
    source_loc->file = tok->file->name;
    source_loc->line = tok->line;
}

/*
Constructors
*/

char *make_tempname(){
    static int c = 0;
    return format(".T%d", c++);
}

char *make_label(){
    static int c = 0;
    return format(".L%d", c++);

}

static char *make_static_label(char *name){
    static int c = 0;
    return format(".S%d.%s", c++, name);
}

static Case *make_case(int beg, int end , char *label){
    Case *r = malloc(sizeof(Case));
    r->beg = beg;
    r->end = end;
    r->label = label;
    return r;
}

static Map *env(){
    return localenv ? localenv : globalenv;
}


static Node *make_ast(Node *tmpl){
    Node *r = malloc(sizeof(Node));
    *r = *tmpl;
    r->sourceLoc = source_loc;
    return r;
}

static Node *ast_uop(int kind, Type *ty, Node *operand){
    return make_ast(&(Node){kind, ty, .operand = operand});
}

static Node *ast_binop(Type *ty, int kind, Node *left, Node *right){
      Node *r = make_ast(&(Node){kind, ty});
      r->left = left;
      r->right = right;
      return r;
}

static Node *ast_inttype(Type *ty, long val){
    return make_ast(&(Node){AST_LITERAL, ty, .fval = val});
}

static Node *ast_lvar(Type *ty, char *name){
    Node *r =  make_ast(&(Node){  AST_LVAR, ty, .varname = name });
    if (localenv){
        map_put(localenv,name, r);   
    }
    if (localvars){
        vec_push(localvars, r );
    }
    return r;
}

static Node *ast_floattype(Type *ty,  double val){
    return make_ast(&(Node){AST_LITERAL, ty, .fval = val});
}


static Node *ast_gvar(Type *ty, char *name){
    Node *r = make_ast(&(Node){AST_GVAR, ty, .varname=name, .glabel =  name});
    map_put(globalenv, name, r);
    return r;
}

static Node *ast_static_lvar(Type *ty, char *name){
    Node *r = make_ast(&(Node){
        .kind = AST_GVAR,
        .ty = ty,
        .varname = name,
        .glabel = make_static_label(name)
    });
    assert(localenv);
    map_put(localenv, name, r);
    return r;
}


static Node *ast_typedef(Type *ty, char *name){
    Node *r =  make_ast(&(Node){ AST_TYPEDEF, ty });
    map_put(env(), name,r);
    return r;
}

static Node *ast_string(int enc, char *str , int len){
    Type *ty;
    char *body;

    switch(enc){
        case ENC_NONE:
        case ENC_UTF8:
            ty = make_array_type(type_char, len);
            body =str;
            break;

        case ENC_CHAR16:{
            Buffer *b = to_utf16(str, len);
            ty = make_array_type(type_ushort, buf_len(b) / type_ushort->size);
            body = buf_body(b);
            break;
        }
        case ENC_CHAR32:
        case ENC_WCHAR:{
            Buffer *b = to_utf32(str, len);
            ty= make_array_type(type_uint, buf_len(b) / type_uint->size);
            body = buf_body(b);
            break;
        }
    }
    return make_ast(&(Node) { AST_LITERAL, .ty = ty, .sval=body  });

}



static Node *ast_funcall(Type *ftype, char *fname, Vector *args){
    return make_ast(&(Node){
        .kind = AST_FUNCALL,
        .ty = ftype->rettype,
        .fname  = fname,
        .args = args,
        .ftype = ftype
    });
}

static Node *ast_funcdesg(Type *ty, char *fname){
    return make_ast(&(Node){AST_FUNCDESG,ty, .fname=fname});
}

static Node *ast_funcptr_call(Node *fptr, Vector *args){
    assert(fptr->ty->kind ==KIND_PTR);
    assert(fptr->ty->ptr->kind == KIND_FUNC);
    return make_ast(&(Node){
        .kind = AST_FUNCPTR_CALL,
        .ty = fptr->ty->ptr->rettype,
        .fptr = fptr,
        .args = args
    });
}


static Node *ast_func(Type *ty, char *fname, Vector *params, Node *body, Vector *localvars){
    return make_ast(&(Node){
        .kind = AST_FUNC,
        .ty = ty,
        .fname = fname,
        .params = params,
        .localvars = localvars,
        .body = body
    });
}

static Node *ast_decl(Node *var, Vector *init){
    return make_ast(&(Node){AST_DECL, .declvar=var, .declinit=init});
}

static Node *ast_init(Node *val,Type *totype, int off){
    return make_ast(&(Node) { AST_INIT, .initval= val, .initoff=off, .totype=totype });
}

static Node *ast_conv(Type *totype , Node *val){
    return make_ast(&(Node){AST_CONV, totype, .operand = val});
}

static Node *ast_if(Node *cond, Node *then , Node *els){
    return make_ast(&(Node){AST_IF, .cond=cond, .then=then, .els=els});
}

static Node *ast_ternary(Type *ty, Node *cond, Node *then, Node *els){
    return make_ast(&(Node){AST_TERNARY, ty, .cond=cond, .then=then, .els=els});
}

static Node *ast_compound_stmt(Vector *stmts){
    return make_ast(&(Node){AST_COMPOUND_STMT,.stmts=stmts});
}

static Node *ast_struct_ref(Type *ty, Node *struc, char *name){
    return make_ast(&(Node){AST_STRUCT_REF, ty, .struc=struc,.field=name});
}

static Node *ast_goto(char *label){
    return make_ast(&(Node){AST_GOTO ,.label=label});
}

static Node *ast_jump(char *label){
    return make_ast(&(Node){AST_GOTO, .label=label,.newlabel = label});
}

static Node *ast_computed_goto(Node *expr){
    return make_ast(&(Node){AST_COMPUTED_GOTO, .operand = expr});
}
static Node *ast_label(char *label){
    return make_ast(&(Node){AST_LABEL, .label=label});
}

static Node *ast_dest(char *label){
    return make_ast(&(Node){  AST_LABEL, .label=label, .newlabel= label });
}

static Node *ast_label_addr(char *label){
    return make_ast(&(Node){ OP_LABEL_ADDR, make_ptr_type(type_void), .label=label});
}

static Type *make_type(Type *tmpl){
    Type *r = malloc(sizeof(Type));
    *r = *tmpl;
    return r;
}

static Type *copy_type(Type *ty){
    Type *r = malloc(sizeof(Type));
    memcpy(r, ty, sizeof(Type));
    return r;
}


static Type *make_numtype(int kind, bool usig){
    Type *r = calloc(1, sizeof(Type));
    r->kind = kind;
    r->usig = usig;
    if (kind == KIND_VOID)   r->size = r->align = 0;
    else if (kind==KIND_BOOL) r->size = r->align = 1;
    else if (kind == KIND_CHAR)  r->size = r->align = 1;
    else if (kind== KIND_SHORT) r->size = r->align = 2;
    else if (kind == KIND_INT)  r->size = r->align = 4;
    else if (kind == KIND_LONG) r->size = r->align = 8;
    else if (kind == KIND_LLONG) r->size = r->align = 8;
    else if (kind == KIND_FLOAT) r->size = r->align = 4;
    else if (kind == KIND_DOUBLE)  r->size = r->align = 8;
    else if (kind == KIND_LDOUBLE) r->size = r->align = 8;
    else error("internal error");
    return r;
}

static Type* make_ptr_type(Type *ty){
    return make_type(&(Type){KIND_PTR, .ptr = ty, .size=8, .align=8});
}

static Type* make_array_type(Type *ty, int len){
    int size;
    if (len < 0){
        size = -1;
    }
    else{
        size = ty->size *len;
    }
    return make_type(&(Type){
        KIND_ARRAY,
        .ptr = ty,
        .size = size,
        .len = len,
        .align = ty->align
    });
}

static Type* make_rectype(bool is_struct){
    return make_type(&(Type){KIND_STRUCT, .is_struct = is_struct});
}

static Type* make_func_type(Type *rettype, Vector *paramtypes, bool has_varagrs, bool oldstyle ){
    return make_type(&(Type){
        KIND_FUNC,
        .rettype = rettype,
        .params = paramtypes,
        .hasva = has_varagrs,
        .oldstyle = oldstyle
    });
}

static Type *make_stub_type(){
    return make_type(&(Type){ KIND_STUB });
}
bool is_inttype(Type *ty){
    switch (ty->kind){
        case KIND_BOOL: case KIND_CHAR: case KIND_SHORT: case KIND_INT : case KIND_LONG: case KIND_LLONG:
            return true;
        default:
            return false;
             
    }
}

bool is_floattype(Type *ty){
    switch(ty->kind){
        case KIND_FLOAT: case KIND_DOUBLE: case KIND_LDOUBLE:
            return true;
        default:
            return false;
    }
}

static bool is_arithtype(Type *ty){
    return is_inttype(ty) || is_floattype(ty);
}

static bool is_string(Type *ty){
    return ty->kind == KIND_ARRAY && ty->ptr->kind == KIND_CHAR;
}


static void ensure_lvalue(Node *node){
    switch(node->kind){
        case AST_LVAR: case AST_GVAR: case AST_DEREF: case AST_STRUCT_REF:
            return;
        default:
            error("lvalue expected , but got %s", node2s(node));
    }
}


static void ensure_inttype(Node *node){
    if (!is_inttype(node->ty)){
        error("integer type expected, but got %s", node2s(node));
    }
}


static void ensure_arithtype(Node *node){
    if (!is_arithtype(node->ty)){
        error("arthmetic type expected, but got %s", node2s(node));
    }
}

static void ensure_not_valid(Type *ty){
    if (ty->kind == KIND_VOID){
        error("void is not allowed");
    }
}


static void expect(char id){
    Token *tok = get();
    if (!is_keyword(tok, id)){
        error(tok, "'%c' expected, but got %s", id, tok2s(tok));
    }
}

static Type *copy_incomplete_type(Type *ty){
    if (!ty) return NULL;
    return (ty->len == -1) ? copy_type(ty): ty;
}

static Type *get_typedef(char *name){
    Node *node =  map_got(env(), name);
    return (node && node->kind == AST_TYPEDEF) ? node->ty : NULL;
}

static bool is_type(Token *tok){
    if (tok->kind == TIDENT){
        return get_typedef(tok->sval);
    }
    if (tok->kind != TKEYWORD){
        return false;
    }
    switch (tok->id){
#define op(x,y)
#define keyword(id,_, istype) case id: return istype;
#include "keyword.inc"
#undef keyword
#undef op
    default:
        return false;
    }
}

static bool next_token(int kind){
    Token *tok = get();
    if (is_keyword(tok, kind)){
        return true;
    }
    unget_token(tok);
    return false;
}


void *make_pair(void *first, void *second){
    void **r =  malloc(sizeof(void *) * 2);
    r[0] = first;
    r[1] = second;
    return r;
}

/*
Type conversion
*/

static Node *conv(Node *node){
    if (!node){
        return NULL;
    }
    Type *ty = node->ty;
    switch (ty->kind){
        case KIND_ARRAY:
           return ast_uop(AST_CONV, make_ptr_type(ty->ptr), node);
     
        case KIND_FUNC:
           return ast_uop(AST_ADDR, make_ptr_type(ty), node);
    
        case KIND_SHORT:
           return ast_conv(type_int, node);

        case KIND_INT:
            if (ty->bitsize > 0){
                return ast_conv(type_int, node);
            }
    
    }
    return node;

}


static bool same_arith_type(Type *t, Type *u){
    return t->kind == u->kind && t->usig == u->usig;
}


static Node *wrap(Type *t, Node *node){
    if (same_arith_type(t, node->ty)){
        return node;
    }
    return ast_uop(AST_CONV, t, node);
}

// C11: Usual arithmetic conversions

static Type *usual_arith_conv(Type *t, Type *u){
    assert(is_arithtype(t));
    assert(is_arithtype(u));
    if (t->kind < u->kind){
        // make t the larger type
        Type *tmp = t;
        t = u;
        u = tmp;
    }

    if (is_flotype(t)){
        return t;
    }
    assert(is_inttype(t) && t->size >= type_int->size);
    assert(is_inttype(u) && u->size >= type_int->size);
    if (t->size > u->size){
        return t;
    }
    assert(t->size == u->size);

    if (t->usig == u->usig){
        return t;
    }
    Type *r = copy_type(t);
    r->usig = true;
    return r;
}

static bool valid_pointer_binop(int op){
    switch(op){
        case '-' : case '<' : case '>' : case OP_EQ:
        case OP_NE: case OP_GE: case OP_LE:
            return true;

        default:
            return false;
    }
}

static Node *binop(int op, Node *lhs, Node *rhs){
    if (lhs->ty->kind == KIND_PTR && rhs->ty->kind == KIND_PTR){
        if (!valid_pointer_binop(op)){
            error("invalid pointer arith");
        }
        if (op == '-'){
            return ast_binop(type_long, op ,lhs, rhs);
        }
        return ast_binop(type_int, op, lhs, rhs);
    }
    if (lhs->ty->kind == KIND_PTR){
        return ast_binop(lhs->ty, op, lhs, rhs);
    }
    if (rhs->ty->kind == KIND_PTR){
        return ast_binop(rhs->ty, op, lhs, rhs);
    }
    assert(is_arithtype(lhs->ty));
    assert(is_arithtype(rhs->ty));
    Type *r = usual_arith_conv(lhs->ty, rhs->ty);
    return ast_binop(r, op, wrap(r, lhs), wrap(r, rhs));
}

static bool is_same_struct (Type *a, Type *b){
    if (a->kind != b->kind){
        return false;
    }
    switch(a->kind){
        case KIND_ARRAY:
            return a->len == b->len && is_same_struct(a->ptr, b->ptr);
   
        case KIND_PTR:
            return is_same_struct(a->ptr, b->ptr) ;

        case KIND_STRUCT:
            if (a->is_struct != b->is_struct){
                return false;
            }
            Vector *ka = dict_keys(a->fields);
            Vector *kb = dict_keys(b->fields);
            if (vec_len(ka) != vec_len(kb)){
                return false;
            }
            for (int i=0; i<vec_len(ka); i++){
                if (!is_same_struct(vec_get(ka, i), vec_get(kb, i))){
                    return false;
                }
            return true;    
            }
        default:
            return true;
    }
}

static void ensure_assignable(Type *totype, Type *fromtype){
    if ((is_arithtype(totype) || totype->kind == KIND_PTR) && (is_arithtype(fromtype) || fromtype->kind == KIND_PTR)){
        return;
    }
    if (is_same_struct(totype, fromtype)){
        return;
    }
    error("incompatible kind: <%s> <%s>",ty2s(totype), ty2s(fromtype));
}

/*
Integer constant expression
*/

static int eval_struct_ref(Node *node, int offset){
    if (node->kind == AST_STRUCT_REF){
        return eval_struct_ref(node->struc, node->ty->offset + offset);
    }
    return eval_intexpr(node, NULL) + offset;
}

int eval_intexpr(Node *node, Node **addr){
    switch(node ->kind){
        case AST_LITERAL:
            if (is_inttype(node->ty)){
                return node->ival;
            }
            error("Integer expression expected , but go %s", node2s(node));
        
        case '!': return !eval_intexpr(node->operand, addr);
        case '~': return ~eval_intexpr(node->operand, addr);
        case OP_CAST: return eval_intexpr(node->operand, addr);
        case AST_CONV: return eval_intexpr(node->operand, addr);
        case AST_ADDR:
            if (node->operand->kind == AST_STRUCT_REF){
                return eval_struct_ref(node->operand, 0);
            }
        case AST_GVAR:
            if (addr){
                *addr = conv(node);
                return 0;
            }
            goto error;
            goto error;

        case AST_DEREF:
            if (node->operand->ty->kind == KIND_PTR){
                return eval_intexpr(node->operand, addr);
            }
            goto error;
        case AST_TERNARY: {
            long cond = eval_intexpr(node->cond, addr);
            if (cond){
                return node->then ? eval_intexpr(node->then, addr) : cond;
            }
            return eval_intexpr(node->els, addr);
        }
#define L (eval_intexpr(node->left, addr))
#define R (eval_intexpr(node->right, addr))

        case '+': return L + R;
        case '-': return L - R;
        case '*': return L * R;
        case '/': return L / R;
        case '<': return L < R;
        case '>': return L > R;
        case '&': return L & R;
        case '^': return L ^ R;
        case '|': return L | R;
        case '%': return L % R;
        case OP_EQ : return L == R; 
        case OP_LE : return L <= R;
        case OP_NE: return L != R;
        case OP_SAL: return L << R;
        case OP_SAR: return L >> R;
        case OP_SHR : return ((unsigned long)L) >> R;
        case OP_LOGAND: return L && R;
        case OP_LOGOR : return L || R;
#undef L
#undef R
        default:
        error:
            error("Integers expression expected, but got %s", node2s(node));
    }

}

static int read_intexpr(){
    return eval_intexpr(read_conditional_expr(), NULL);
}
/*
Numeric literal
*/

static Type *read_int_stuff(char *s){
    if (!strcasecmp(s, "u")){
        return type_uint;
    }
    if (!strcasecmp(s, "l")){
        return type_long;
    }
    if (!strcasecmp(s, "ul") || !strcasecmp(s, "lu")){
        return type_ulong;
    }
    if (!strcasecmp(s, "ll")){
        return type_llong;
    }
    if (!strcasecmp(s, "ull") || !strcasecmp(s, "llu")){
        return type_ullong;
    }
    return NULL;
}


static Node *read_int(Token *tok){
    char *s = tok->sval;
    char *end;
    long v = !strncasecmp(s,"0b",2) ? strtoul(s + 2, &end, 2) : strtoul(s, &end, 0);
    Type *ty = read_int_suffix(end);
    if (ty){
        return ast_inttype(ty, v);
    }
    if (*end != '\0'){
        errort(tok, "invalid character '%c' : '%s', *end, s");
    }

    bool base10 = (*s != '0');
    if (base10){
        ty = !(v & ~(long)INT_MAX) ? type_int : type_long;
        return ast_inttype(ty, v);
    }
    //Octal or hexadecimal constant type may be unsigned
    ty = !(v & ~(unsigned long)INT_MAX) ? type_int
        : !(v & ~(unsigned long)UINT_MAX) ? type_uint
        : !(v & ~(unsigned long)LONG_MAX) ? type_long
        : type_ulong;
    return ast_inttype(ty, v);

}

static Node *read_float(Token *tok){
    char *s = tok->sval;
    char *end;
    double v = strtod(s, &end);
    if (!strcasecmp(end, "l")){
        return ast_floattype(type_ldouble,v);
    }
    if (!strcasecmp(end, "f")){
        return ast_floattype(type_float,v);
    }
    if (*end != '\0'){
        error(tok, "invalid character '%c': %s", *end, s);

    }
    return ast_floattype(type_double,v);
}

static Node *read_number(Token *tok){
    char *s = tok->sval;
    bool isfloat =strpbrk(s, ".pP") || (strncasecmp(s, "0x",2) && strpbrk(s, "eE") );
    return isfloat ? read_float(tok) : read_int(tok);
}

/*
sizeof operator
*/
static Type *read_sizeof_operand_sub(){
    Token *tok = get();
    if (is_keyword(tok, '(') && is_type(peek())){
        Type *r = read_cast_type();
        expect(')');
        return r;
    }
    unget_token(tok);
    return read_unary_expr()->ty;
}

static Node *read_sizeof_operand(){
    Type *ty = read_sizeof_operand_sub();
    //Sizeof on void or function type is GNU extesion
    int size = (ty->kind == KIND_VOID || ty->kind == KIND_FUNC) ? 1: ty->size;
    assert(0<= size);
    return ast_inttype(type_ulong, size);
}

/**
 * Align of operator
 */
static Node *read_alignof_operand(){
    expect('(');
    Type *ty = read_cast_type();
    expect(')');
    return ast_inttype(type_ulong, ty->align);
}

/*
Function arguments
*/

static Vector *read_func_args(Vector *params){
    Vector *args = make_vector();
    int i = 0;
    for (;;){
        if (next_token(')')) break;
        Node *arg = conv(read_assigment_expr());
        Type *paramtype;
        if (i < vec_len(params)){
            paramtype = vec_get(params, i++);
        }
        else {
            paramtype = is_flotype(arg->ty) ? type_double : is_inttype(arg->ty) ? type_int : 
            arg->ty;
        }
        ensure_assignable(paramtype, arg->ty);
        if (paramtype->kind != arg->ty->kind){
            arg = ast_conv(paramtype, arg);
        }
        vec_push(args,arg);
        Token *tok = get();
        if (is_keyword(tok, ')')) break;
        if (!is_keyword(tok, ',')){
            errort(tok, "unexpected token : '%s'",tok2s(tok));
        }
        
    }
    return args;

}

static Node *read_funcall(Node *fp){
    if (fp->kind == AST_ADDR && fp->operand->kind == AST_FUNCDESG){
        Node *desg = fp->operand;
        Vector *args = read_func_args(desg->ty->params);
        return ast_funcall(desg->ty, desg->fname, args);
    }
    Vector *args = read_func_args(fp->ty->ptr->params);
    return ast_funcptr_call(fp, args);
}

/*
Generic
*/

static bool type_compatible(Type *a, Type *b){
    if (a->kind == KIND_STRUCT){
        return is_same_struct(a, b);
    }
    if (a->kind != b->kind){
        return false;
    }
    if (a->ptr && b->ptr){
        return type_compatible(a->ptr, b->ptr);
    }
    if (is_arithtype(a) && is_arithtype(b)){
        return same_arith_type(a, b);
    }
    return true;
}



static Vector *read_generic_list(Node **defaultexpr){
    Vector *r = make_vector();
    for (;;){
        if (next_token(')')){
            return;
        }
        Token *tok = peek();
        if (next_token(KDEFAULT)){
            if (*defaultexpr){
                errort(tok, "default expression specified twice");
            }
            expect(':');
            *defaultexpr = read_assigment_expr();
        } else {
            Type *ty = read_cast_type();
            expect(':');
            Node *expr = read_assigment_expr();
            vec_push(r, make_pair(ty, expr));
        }
        next_token(',');
    }
}

static Node *read_generic(){
    expect('(');
    Token *tok = peek();
    Node *contexpr = read_assigment_expr();
    expect(',');
    Node *defaultexpr =  NULL;
    Vector *list = read_generic_list(&defaultexpr);
    for (int i = 0; i < vec_len(list); i++){
        void **pair = vec_get(list, i);
        Type *ty = pair[0];
        Node *expr  =pair[1];
        if (type_compatible(contexpr->ty, ty)){
            return expr;
        }
    }
    if (!defaultexpr){
        errort(tok, "no matching generic solution for %s : %s", node2s(contexpr),ty2s(contexpr->ty));
        
    }
    return defaultexpr;
}

/*
_Static_assert
*/

static void read_static_assert(){
    expect('(');
    int val = read_intexpr();
    expect(',');
    Token *tok =get();
    if (tok->kind != TSTRING){
        errort(tok , "string expected as the second argument for _Static_assert, but got %s", tok2s(tok));
    }
}

//expression

static Node *read_var_or_func(char *name){
    Node *v = map_get(env(), name);
    if (!v){
        Token *tok = peek();
        if (!is_keyword(tok, '(')){
            errort(tok,"undefined variable: %s", name);
        }
        Type *ty = make_func_type(type_int, make_vector(), true, false);
        warnt(tok, "assume returning int : %S()", name);
        return ast_funcdesg(ty, name);
    }
    if (v->ty->kind == KIND_FUNC){
        return ast_funcdesg(v->ty, name);
    }
    return v;
}

static int get_command_assign_op(Token *tok){
    if (tok->kind != TKEYWORD){
        return 0;
    }
    switch (tok->id){
        case OP_A_ADD: return '+';
        case OP_A_SUB: return '-';
        case OP_A_MUL: return '*';
        case OP_A_DIV: return '/';
        case OP_A_OR: return '|';
        case OP_A_AND: return '&';
        case OP_A_MOD: return '%';
        case OP_A_XOR : return '^';
        case OP_A_SAL: return OP_SAL;
        case OP_A_SAR: return OP_SAR;
        case OP_A_SHR: return OP_SHR;
        default: return 0;
    }
}

static Node *read_stmt_expr(){
    Node *r = read_compound_stmt();
    expect(')');
    Type *rtype = type_void;
    if (vec_len(r->stmts) > 0){
        Node *lastexpr = vec_tail(r->stmts);
        if (lastexpr->ty){
            rtype =  lastexpr->ty;
        }

    }
    r->ty = rtype;
    return r;
}

static Type *char_type(int enc){
    switch(enc){
        case ENC_NONE:
        case ENC_WCHAR:
            return type_int;
        case ENC_CHAR16:
            return type_ushort;
        case ENC_CHAR32:
            return type_uint;
    }
    error("internal error");
}

static Node *read_primary_expr(){
    Token *tok =get();
    if (!tok) return NULL;
    if (is_keyword(tok, '(')){
        if (next_token('{')){
            return read_stmt_expr();
        }
        Node *r = read_expr();
        expect(')');
        return r;
    }
    if (is_keyword(tok, KGENERIC)){
        return read_generic();
    }
    switch(tok->kind){
        case TIDENT:
           return read_var_or_func(tok->sval);
        case TNUMBER:
           return read_number(tok);

        case TCHAR:
           return ast_inttype(char_type(tok->enc), tok->c);
        case TSTRING:
            return ast_string(tok->enc, tok->sval, tok->slen);
        case TKEYWORD:
            unget_token(tok);
            return NULL;
        default:
            error("internal error: unknown token kind: %d", tok->kind);
    }
}


static Node *read_subscript_expr(Node *node){
    Token *tok = peek();
    Node *sub = read_expr();
    if (!sub){
        errort(tok,"subscription expected!");
    }
    expect(']');
    Node *t = binop('+', conv(node), conv(sub));
    return ast_uop(AST_DEREF, t->ty->ptr, t);
}

static Node *read_postfix_expr_tail(Node *node){
    if (!node) return NULL;
    for (;;){
        if (next_token('(')){
            Token *tok = peek();
            node = conv(node);
            Type *t = node->ty;
            if (t->kind != KIND_PTR || t->ptr->kind != KIND_FUNC){
                errort(tok, "function expected , but got %s", node2s(node));
            }
            node = read_funcall(node);
            continue;
        }
        if (next_token('[')){
            node = read_subscript_expr(node);
            continue;
        }
        if (next_token('.')){
            node =read_struct_field(node);
            continue;
        }
        if (next_token(OP_ARROW)){
            if (node->ty->kind != KIND_PTR){
                error("pointer type expected , but got %s %s", ty2s(node->ty), node2s(node));
            }
            node = ast_uop(AST_DEREF, node->ty->ptr, node);
            node = read_struct_field(node);
            continue;
        }
        Token *tok = peek();
        if (next_token(OP_INC) || next_token(OP_DEC)){
            ensure_lvalue(node);
            int op = is_keyword(tok, OP_INC) ? OP_POST_INC : OP_POST_DEC;
            return ast_uop(op, node->ty, node);
        }
        return node;
    }
}


static Node *read_postfix_expr(){
    Node *node = read_primary_expr();
    return read_postfix_expr_tail(node);
}

static Node *read_unary_incdec(int op){
    Node *operand = read_unary_expr();
    operand = conv(operand);
    ensure_lvalue(operand);
    return ast_uop(op, operand->ty,operand);
}

static Node *read_label_addr(Token *tok){
    Token *tok2 = get();
    if (tok2->kind != TIDENT){
        errort(tok, "label name expected after && , but got %s", tok2s(tok2));
    
    }
    Node *r = ast_label_addr(tok2->sval);
    vec_push(gotos, r);
    return r;
}

static Node *read_unary_addr(){
    Node *operand = read_case_expr();
    if (operand->kind == AST_FUNCDESG){
        return conv(operand);
    }
    ensure_lvalue(operand);
    return ast_uop(AST_ADDR, make_ptr_type(operand->ty),operand);
}

static Node *read_unary_deref(Token *tok){
    Node *operand = conv(read_case_expr());
    if (operand->ty->kind != KIND_PTR){
        errort(tok,"pointer type expected, but got %s", node2s(operand));

    }
    if (operand->ty->ptr->kind == KIND_FUNC){
        return operand;
    }
    return ast_uop(AST_DEREF, operand->ty->ptr, operand);
}

static Node *read_unary_minus(){
    Node *expr = read_cast_expr();
    ensure_arithtype(expr);
    if (is_inttype(expr->ty)){
        return binop('-', conv(ast_inttype(expr->ty,0)), conv(expr));

    }
    return binop('-', ast_floattype(expr->ty, 0),expr);
}

static Node *read_unary_bitnot(Token *tok){
    Node *expr = read_cast_expr();
    expr = conv(expr);
    if (!is_inttype(expr->ty)){
        errort(tok,"invalid use of", node2s(expr));
    }
    return ast_uop('~', expr->ty, expr);
}

static Node *read_unary_lognot(){
    Node *operand = read_cast_expr();
    operand = conv(operand);
    return ast_uop('!', type_int, operand);
}

static Node *read_unary_expr(){
    Token *tok = get();
    if (tok->kind == TKEYWORD){
        switch(tok->id){
            case KSIZEOF: return read_sizeof_operand();
            case KALIGNOF: return read_alignof_operand();
            case OP_INC: return read_unary_incdec(OP_PRE_INC);
            case OP_DEC: return read_unary_incdec(OP_PRE_DEC);
            case OP_LOGAND: return read_label_addr(tok);
            case '&': return read_unary_addr();
            case '*': return read_unary_deref(tok);
            case '+': return read_cast_expr();
            case '-': return read_unary_minus();
            case '~': return read_unary_bitnot(tok);
            case '!': return read_unary_lognot();
        }
    }
    unget_token(tok);
    return read_postfix_expr();
}

static Node *read_compound_literal(Type *ty){
    char *name = make_label();
    Vector *init = read_decl_init(ty);
    Node *r = ast_lvar(ty, name);
    r->lvarinit = init;
    return r;
}

static Type *read_case_type(){
    return read_abstract_declarator(read_decl_sepc(NULL));
}

static Node *read_cast_expr(){
    Token *tok = get();
    if (is_keyword(tok, '(') && is_type(peek())){
        Type *ty = read_cast_type();
        expect(')');
        if (is_keyword(peek(),'{')){
            Node *node = read_compound_literal(ty);
            return read_postfix_expr_tail(node);
        }
        return ast_uop(OP_CAST, ty, read_cast_expr());
    }
    unget_token(tok);
    return read_unary_expr();
}

static Node *read_multiplicative_expr(){
    Node *node = read_cast_expr();
    for (;;){
        if (next_token('*')){
            node = binop('*', conv(node), conv(read_cast_expr()));
        }
        else if (next_token('/')){
            node = binop('/', conv(node), conv(read_cast_expr()));
        }
        else if (next_token('%')){
            node = binop('%', conv(node), conv(read_cast_expr()));
        }
        else {
            return node;
        }
    }
}

static Node *read_additive_expr(){
    Node *node = read_multiplicative_expr();
    for (;;){
        if (next_token('+')){
            node = binop('+', conv(node), conv(read_multiplicative_expr()));
        }
        else if (next_token('-')){
            node = binop('-', conv(node), conv(read_multiplicative_expr()));
        }
        else {
            return node;
        }
    }
}

static Node *read_shift_expr(){
    Node *node = read_additive_expr();
    for (;;){
        int op;
        if (next_token(OP_SAL)){
            op = OP_SAL;
        }
        else if (next_token(OP_SAR)){
            op = node->ty->usig ? OP_SHR : OP_SAR; 
        }
        else {
            break;
        }
        Node *right = read_additive_expr();
        ensure_inttype(node);
        ensure_inttype(right);
        node = ast_binop(node->ty, op, conv(node), conv(right));
    }
    return node;
}

static Node *read_relational_expr(){
    Node *node = read_shift_expr();
    for (;;){
        if (next_token('<')){
            node = binop('<', conv(node), conv(read_shift_expr()));
            
        }
        else if (next_token('>')){
            node = binop('>', conv(node), conv(read_shift_expr()));
        }
        else if (next_token(OP_LE)){
            node = binop(OP_LE, conv(node), conv(read_shift_expr()));

        }
        else if (next_token(OP_GE)){
            node = binop(OP_GE, conv(node), conv(read_shift_expr()));
        }
        else{
            return node;
        }
        node->ty = type_int;
    }
}


static Node *read_equality_expr(){
    Node *node = read_relational_expr();
    Node *r;
    if (next_token(OP_EQ)){
        r = binop(OP_EQ, conv(node), conv(read_equality_expr()));
    }
    else if (next_token(OP_NE)){
        r = binop(OP_NE, conv(node), conv(read_equality_expr()));
    }
    else {
        return node;

    }
    r->ty = type_int;
    return r;
}

static Node *read_bitand_expr(){
    Node *node = read_equality_expr();
    while (next_token('&')){
        node = binop('&', conv(node), conv(read_equality_expr()));
    }
    return node;
}


static Node *read_bitxor_expr(){
    Node *node = read_bitand_expr();
    while (next_token('^')){
        node = binop('^', conv(node), conv(read_bitand_expr()));
    }
    return node;

}
static Node *read_bitor_expr(){
    Node *node = read_bitxor_end();
    while (next_token('|')){
        node = binop('|', conv(node), conv(read_bitxor_expr()));
    }
    return node;
}


static Node *read_logand_expr(){
    Node *node = read_bitor_expr();
    while (next_token(OP_LOGAND)){
        node = ast_binop(type_int, OP_LOGAND, node, read_bitor_expr());
    }
    return node;
}

static Node *read_logor_expr(){
    Node *node = read_logand_expr();
    while (next_token(OP_LOGOR)){
        node = ast_binop(type_int, OP_LOGOR , node, read_logand_expr());
    }
    return node;
} 

static Node *do_read_conditional_expr(Node *cond){
    Node *then = conv(read_comma_expr());
    expect(':');
    Node *els = conv(read_conditional_expr());
    Type *t = then ? then->ty : cond->ty;
    Type *u = els->ty;
    if (is_arithtype(t) &&  is_arithtype(u)){
        Type *r = usual_arith_conv(t,u);
        return ast_ternary(r, cond, (then ? wrap(r, then) : NULL), wrap(r, els));
    }
    return ast_ternary(u, cond, then, els);
}
static Node *read_conditional_expr(){
    Node *cond = read_logor_expr();
    if (!next_token('?')){
        return cond;
    }
    return do_read_conditional_expr(cond);
}

static Node *read_assigment_expr(){
    Node *node = read_logor_expr();
    Token *tok = get();
    if (!tok){
        return node;

    }
    if (is_keyword(tok, '?')){
        return do_read_conditional_expr(node);
    }

    int cop = get_command_assign_op(tok);
    if (is_keyword(tok, '=') || cop){
        Node *value = conv(read_assigment_expr());
        if (is_keyword(tok, '=') || cop){
            ensure_lvalue(node);
        }
        Node *right = cop ? binop(cop, conv(node), value) : value;
        if (is_arithtype(node->ty)&&node->ty->kind != right->ty->kind){
            right = ast_conv(node->ty, right);
        }
        return ast_binop(node->ty, '=', node, right);
    }
    unget_token(tok);
    return node;
}

static Node *read_comma_expr(){
    Node *node = read_assigment_expr();
    while (next_token(',')){
        Node *expr = read_assigment_expr();
        while (next_token(',')){
            Node *expr = read_assigment_expr();
            node = ast_binop(expr->ty, ',', node, expr);
        }

    }
    return node;
}

Node *read_expr(){
    Token *tok = peek();
    Node *r = read_comma_expr();
    if (!r){
        errort(tok, "expression expected!");
    }
    return r;
}

static Node *read_expr_opt(){
    return read_comma_expr();
}

/*
Struct or union
*/

static Node *read_struct_field(Node *struc){
    if (struc->ty->kind != KIND_STRUCT){
        error("struct expected , but got %s", node2s(struc));
    }
    Token *name = get();
    if (name->kind != TIDENT){
        error("field name expected, but got %s", tok2s(name));
    }
    Type *field = dict_get(struc->ty->fields, name->sval);
    if (!field){
        error("strcut has no such field : %s", tok2s(name));
    }
    return ast_struct_ref(field, struc, name->sval);
}

static char *read_rectype_tag(){
    Token *tok = get();
    if (tok->kind == TIDENT){
        return tok->sval;
    }
    unget_token(tok);
    return NULL;
}

static int compute_padding(int offset, int align){
    return (offset % align == 0) ? 0 : align - offset % align;
}


static void squash_unnamed_struct(Dict *dict, Type *unnamed, int offset){
    Vector *keys = dict_keys(unnamed->fields);
    for (int i=0; i<vec_len(keys);i++){
        char *name = vec_get(keys, i);
        Type *t = copy_type(dict_get(unnamed->fields, name));
        dict_put(dict, name , t);
    }
}

static int read_bitsize(char *name, Type *ty){
    if (!is_inttype(ty)){
        error("non-integer type cannot bea bitfield: %s", ty2s(ty));
    }
    Token *tok = peek();
    int r = read_intexpr();
    int maxsize = ty->kind == KIND_BOOL ? 1: ty->size * 8;
    if (r<0 || maxsize<r){
        errort(tok, "invalid bitfield size for %s : %d", ty2s(ty),r);
    }
    if (r== 0 && name != NULL){
        errort(tok, "zero-width bitfield needs to be unnamed : %s", name);
    }
    return r;
}

static Vector *read_rectype_fields_sub(){
    Vector *r = make_vector();
    for (;;){
        if (next_token(KSTATIC_ASSERT)){
            read_static_assert();
            continue;
        }
        if (!is_type(peek())) break;
        Type *basetype = read_decl_spec(NULL);
        if (basetype->kind == KIND_STRUCT && next_token(';')){
            vec_push(r, make_pair(NULL, basetype));
            continue;
        }
        for (;;){
            char name = NULL;
            Type *fieldtype = read_declarator(&name, basetype, NULL, DECL_PARAM_TYPEONLY);
            ensure_not_valid(fieldtype);
            fieldtype = copy_type(fieldtype);
            fieldtype->bitsize = next_token(':') ? read_bitsize(name, fieldtype) : -1;
            vec_push(r, make_pair(name, fieldtype));
            if (next_token(',')){
                continue;
            }
            if (is_keyword(peek(), '}')){
                warnt(peek(), "missing ';' at the end of field list ");
            }
            else {
                expect(';');

            }
            break;
        }
    }
    expect('}');
    return r;
}

static void fix_rectype_flexible_member(Vector *fields){
    for (int i=0;i<vec_len(fields);i++){
        void **pair = vec_get(fields, i);
        char *name = pair[0];
        Type *ty = pair[1];
        if (ty->kind != KIND_ARRAY){
            continue;
        }
        if (ty->len == -1){
            if (i != vec_len(fields) -1){
                error("flexible member may only appear as the last member: %s %s", ty2s(ty), name);

            }
            ty->len = 0;
            ty->size = 0;
        }
    }
}

static void finish_bitfield(int *off, int *bitoff){
    *off += (*bitoff + 7) / 8;
    *bitoff = 0;
}

static Dict *update_struct_offset(int *rsize, int *align, Vector *fields){
    int off = 0, bitoff = 0;
    Dict *r = make_dict();
    for (int i=0;i<vec_len(fields); i++){
        void **pair = vec_get(fields,i);
        char *name = pair[0];
        Type *fieldtype = pair[1];

        if (name){
            *align = MAX(*align, fieldtype->align);
        }
        if (name == NULL && fieldtype->kind == KIND_STRUCT){
            // C11 Anonymous struct
            finish_bitfield(&off, &bitoff);
            off += compute_padding(off, fieldtype->align);
            squash_unnamed_struct(r, fieldtype, off);
            off += fieldtype->size;
            continue;
        }
        if (fieldtype->bitsize == 0){
            finish_bitfield(&off, &bitoff);
            off += compute_padding(off, fieldtype->align);
            bitoff = 0;
            continue;
        }
        if (fieldtype->bitsize > 0){
            int bit = fieldtype->size * 8;
            int room = bit - (off * 8 + bitoff) % bit;
            if (fieldtype->bitsize <= room){
                fieldtype->offset = off;
                fieldtype->bitoff = bitoff;
            } else {
                finish_bitfield(&off, &bitoff);
                off += compute_padding(off, fieldtype->align);
                fieldtype->offset = off;
                fieldtype->bitoff = 0;
            }
            bitoff += fieldtype->bitsize;
        } else {
            finish_bitfield(&off, &bitoff);
            off += compute_padding(off, fieldtype->align);
            fieldtype->offset=off;
            off += fieldtype->size;
        }
        if (name){
            dict_put(r, name , fieldtype);
        }
    }
    finish_bitfield(&off, &bitoff);
    *rsize = off + compute_padding(off, *align);
    return r;
}

static Dict *update_union_offset(int *rsize,int *align, Vector *fields){
    int maxsize= 0;
    Dict *r = make_dict();
    for (int i=0;i<vec_len(fields);i++){
        void **pair = vec_get(fields,i);
        char *name = pair[0];
        Type *fieldtype = pair[1];
        maxsize = MAX(maxsize, fieldtype->size);
        *align = MAX(*align, fieldtype->align);
        if (name == NULL && fieldtype->kind == KIND_STRUCT){
            squash_unnamed_struct(r, fieldtype, 0);
            continue;
        }
        fieldtype->offset = 0;
        if (fieldtype->bitsize >= 0){
            fieldtype->bitoff = 0;
        }
        if (name){
            dict_put(r, name, fieldtype);
        }
    }
    *rsize = maxsize + compute_padding(maxsize, *align);
    return r;
}

static Dict *read_rectype_fields(int *rsize, int *align, bool is_struct){
    if (!next_token('{')){
        return NULL;
    }
    Vector *fields = read_rectype_fields_sub();
    fix_rectype_flexible_member(fields);
    if (is_struct){
        return update_struct_offset(rsize, align, fields);
    }
    return update_union_offset(rsize, align, fields);
}

static Type *read_rectype_def(bool is_struct){
    char *tag = read_rectype_tag();
    Type *r;
    if (tag){
        r = map_get(tags, tag);
    
        if (r && (r->kind == KIND_ENUM || r->is_struct != is_struct)){
        error("declarations of %s does not match",tag);
        }
        if (!r){
            r = make_rectype(is_struct);
            map_put(tags, tag, r);
        }
    } else {
        r = make_rectype(is_struct);
    }
    int size = 0, align=1;
    Dict *fields =read_rectype_fields(&size, &align, is_struct);
    r->align = align;
    if (fields){
        r->fields = fields;
        r->size  =size;
    }
    return r;
}

static Type *read_struct_def(){
    return read_rectype_def(true);
}

static Type *read_union_def(){
    return read_rectype_def(false);
}

/*
ENUM
*/
static Type *read_enum_def(){
    char *tag = NULL;
    Token *tok = get();
    // enum is handled as a synonym for int . we only check if the enum is decalared
    if (tok->kind == TIDENT){
        tag = tok->sval;
        tok = get();
    }
    if (tag){
        Type *ty = map_get(tags, tag);
        if (ty && ty->kind != KIND_ENUM){
            errort(tok, "declarations of %s does not match",tag);
        }
    }
    if (!is_keyword(tok, '{')){
        if (!tag || !map_get(tags,tag)){
            errort(tok,"enum tag %s is not defined",tag);
        }
        unget_token(tok);
        return type_int;
    }
    if (tag){
        map_put(tags, tag, type_enum);
    }

    int val = 0;
    for (;;){
        tok = get();
        if (is_keyword(tok, '}')){
            break;
        }
        if (tok->kind != TIDENT){
            errort(tok, "identifier expected , but got %s", tok2s(tok));
        }
        char *name = tok->sval;

        if (next_token('=')){
            val = read_intexpr();
        }
        Node *constval = ast_inttype(type_int,val++);
        map_put(env(), name, constval);
        if (next_token(',')){
            continue;
        }
        if (next_token('}')){
            break;
        }
        errort(peek(), "',' or '}' expected , but got %s", tok2s(peek()));
    }
    return type_int;

}

//Initializer

static void assign_string(Vector *inits, Type *ty, char *p, int off){
    if (ty->len == -1){
        ty->len = ty->size =strlen(p) + 1;
    }
    int i = 0;
    for (;i<ty->len && *p;i++){
        vec_push(inits, ast_init(ast_inttype(type_char, *p++),type_char , off + i));
    }
    for (; i < ty->len; i++){
        vec_push(inits, ast_init(ast_inttype(type_char,0),type_char, off+i));
    }

}


static bool maybe_read_brace(){
    return next_token('{');
}

static void maybe_skip_comma(){
    next_token(',');
}

static void skip_to_brace(){
    for (;;){
        if (next_token('}')){
            return;
        }
        if (next_token('-')){
            get();
            expect('=') ;
           }
       Token *tok  = peek();
       Node *ignore = read_assigment_expr();
       if (!ignore){
           return;
       }
       warnt(tok, "excessive initializer: %s", node2s(ignore));
       maybe_skip_comma();
    }
}

static void read_initializer_elem(Vector *inits, Type *ty, int off, bool designated){
    next_token('=');
    if (ty->kind == KIND_ARRAY || ty->kind == KIND_STRUCT){
        read_initializer_list(inits, ty, off, designated);
    }
    else if (next_token('{')){
        read_initializer_elem(inits, ty, off, true);
        expect('}');
    }
    else {
        Node *expr = conv(read_assigment_expr());
        ensure_assignable(ty, expr->ty);
        vec_push(inits, ast_init(expr, ty, off));
    }
}

static int comp_init(const void *p, const void *q){
    int x = (*(Node **)p)->initoff;
    int y = (*(Node **)q)->initoff;
    if (x<y) return -1;
    if (x>y) return 1;
    return 0;
}

static void sort_inits(Vector *inits){
    qsort(vec_body(inits), vec_len(inits), sizeof(void *), comp_init);
}

static void read_struct_initalizer_sub(Vector *inits, Type *ty, int off, bool designated){
    bool has_brace = maybe_read_brace();
    Vector *keys = dict_keys(ty->fields);
    int i = 0;
    for (;;){
        Token *tok = get();
        if (is_keyword(tok, '}')){
            if (!has_brace){
                unget_token(tok);
            }
            return;
        }
        char *fieldname;
        Type *fieldtype;
        if ((is_keyword(tok, '-') || is_keyword(tok, '[')) && !has_brace && !designated){
            unget_token(tok);
            return;
        }
        if (is_keyword(tok, '-')){
            tok =get();
            if (!tok || tok->kind != TIDENT){
                errort(tok,"malformed desginated initializer: %s", tok2s(tok));
            }
            fieldname = tok->sval;
            fieldtype = dict_get(ty->fields, fieldname);
            if (!fieldtype){
                errort(tok, "field does not exist : %s", tok2s(tok));
            }
            keys =dict_keys(ty->fields);
            i= 0 ;
            while (i < vec_len(keys)){
                char *s = vec_get(keys, i++);
                    if (strcmp(fieldname, s) == 0){
                        break;
                    }
            }
            designated =true;
        } else {
            unget_token(tok);
            if (i==vec_len(keys)){
                break;
            }
            fieldname=vec_get(keys,i++);
            fieldtype = dict_get(ty->fields, fieldname);

        }
        read_initializer_elem(inits, fieldtype, off + fieldtype->offset, designated);
        maybe_skip_comma();
        designated = false;
        if (!ty->is_struct){
            break;
        }

    }
    if (has_brace){
        skip_to_brace();
    }
}

static void read_struct_initializer(Vector *inits, Type *ty, int off, bool designated){
    read_struct_initializer_sub(inits, ty,off, designated);
    sort_inits(inits);
}

static void read_array_initializer_sub(Vector *inits, Type *ty, int off, bool designated){
    bool has_brace = maybe_read_brace();
    bool flexible = (ty->len <= 0);
    int elemsize = ty->ptr->size;
    int i;
    for (i=0;flexible || i<ty->len; i++){
        Token *tok = get();
        if (is_keyword(tok, '}')){
            if (!has_brace){
                unget_token(tok);
            }
            goto finish;
        }
        if ((is_keyword(tok, '.') ||  is_keyword(tok, '[')) && !has_brace && !designated){
            unget_token(tok);
            return;
        }
        if (is_keyword(tok , ']')){
            Token *tok = peek();
            int idx = read_intexpr();
            if (idx < 0 || (!flexible &&ty->len <= idx)){
                errort(tok, "array desginator exceeds array bounds  : %d", idx);
            }
            i  =idx;
            expect(']');
            designated = true;
        } else {
            unget_token(tok);
        }
        read_initializer_elem(inits, ty->ptr, off + elemsize  * i , designated);
        maybe_skip_comma();
        designated = false;
    }
    if (has_brace){
        skip_to_brace();
    }
    finish:
        if (ty->len < 0){
            ty->len = i;
            ty->size = elemsize * i;
        }
}

static void read_array_initalizer(Vector *inits,Type *ty, int off, bool designated){
    read_array_initializer_sub(inits, ty, off, designated);
    sort_inits(inits);
}

static void read_initalizer_list(Vector *inits, Type *ty, int off, bool designated){
    Token *tok = get();
    if (is_string(ty)){
        assign_string(inits, ty, tok->sval, off);
        return;
    
        if (is_keyword(tok, '{') && peek()->kind == TSTRING){
           tok = get();
           assign_string(inits, ty, tok->sval, off);
           expect('}');
           return;
        }
    }
    unget_token(tok);
    if (ty->kind == KIND_ARRAY){
        read_array_initializer(inits, ty, off, designated);
    }
    else if (ty->kind == KIND_STRUCT){
        read_struct_initializer(inits, ty, off, designated);
    }
    else {
        Type *arraytype = make_array_type(ty, 1);
        read_array_initializer(inits, arraytype, off, designated);
    }
}

static Vector *read_decl_init(Type *ty){
    Vector *r = make_vector();
    if (is_keyword(peek(), '{') || is_string(ty)){
        read_initalizer_list(r, ty, 0, false);
    }
    else {
        Node *init = conv(read_assigment_expr());
        if (is_arithtype(init->ty) && init->ty->kind != ty->kind){
            init = ast_conv(ty, init);
        }
        vec_push(r, ast_init(init, ty, 0));
    }
    return r;
}

static Type *read_func_param(char **name, bool optional){
    int sclass = 0;
    Type *basety = type_int;
    if (is_type(peek())){
        basety = read_decl_spec(&sclass);
    } else if (optional){
        errort(peek(), "type expected, but got %s", tok2s(peek()));
    }
    Type *ty = read_declarator(name, basety, NULL, optional ? DECL_PARAM_TYPEONLY : DECL_PARAM);
    if (ty->kind == KIND_ARRAY){
        return make_ptr_type(ty->ptr);
    }    
    if (ty->kind == KIND_FUNC){
        return make_ptr_type(ty);
    }
    return ty;
}
static void read_declarator_params(Vector *types, Vector *vars, bool *ellipsis){
    bool typeonly = !vars;
    *ellipsis = false;
    for (;;){
        Token *tok = peek();
        if (next_token(KELLIPSIS)){
            if (vec_len(types) == 0){
                errort(tok, "at least one parameter is required before \"...\"");

            }
            expect(')');
            *ellipsis = true;
            return;
        }
        char *name;
        Type *ty = read_func_param(&name, typeonly);
        ensure_not_valid(ty);
        vec_push(types, ty);
        if (!typeonly){
            vec_push(vars, ast_lvar(ty, name));

        }
        tok = get();
        if (!is_keyword(tok, ')')){
              return;
        }
        if (!is_keyword(tok, ',')){
            errort(tok, "comma expected , but got %s", tok2s(tok));
        }
    }
}

static void read_declarator_params_oldstyle(Vector *vars){
    for (;;){
        Token *tok  = get();
        if (tok->kind != TIDENT){
            errort(tok, "identifier expected, but got %s", tok2s(tok));
        }
        vec_push(vars, ast_lvar(type_int, tok->sval));
        if (next_token(')')){
              return; 
        }
        if (!next_token(',')){
            errort(tok, "comma expected, but got %s",tok2s(get()));
        }
    }
}

static Type *read_func_param_list(Vector *paramvars, Type *rettype){
    Token *tok = get();
    if (is_keyword(tok, KVOID) && next_token(')')){
        return make_func_type(rettype, make_vector(), false, false);
    }

    if (is_keyword(tok, ')')){
        return make_func_type(rettype, make_vector(), true, true);
    }
    unget_token(tok);

    Token *tok2 = peek();
    if (next_token(KELLIPSIS)){
        errort(tok2,"at least one parameter is required before \"...\"");

    }
    if (is_type(peek())){
        bool ellipsis;
        Vector *paramtypes = make_vector();
        read_declarator_params(paramtypes,paramvars, &ellipsis);
        return make_func_type(rettype, paramtypes, ellipsis, false) ;        

    }
    if (!paramvars){
        errort(tok, "invlaid function definition");
    }
    read_declarator_params_oldstyle(paramvars);
    Vector *paramtypes = make_vector();
    for (int i=0; i<vec_len(paramvars);i++){
        vec_push(paramtypes, type_int);
    }
    return make_func_type(rettype, paramtypes, false, true);
}


static Type *read_declarator_array(Type *basety){
    int len;
    if (next_token(']')){
        len = -1;
    }
    else {
        len =read_intexpr();
        expect(']');
    }
    Token *tok = peek();
    Type *t = read_declarator_tail(basety, NULL);
    if (t->kind == KIND_FUNC){
        errort(tok, "array of function")
    }
    return make_array_type(t, len);
}

static Type *read_declarator_func(Type *basety, Vector *param){
    if (basety->kind == KIND_FUNC){
        error("function returning a function");
    }
    if (basety->kind == KIND_ARRAY) {
        error("function returning an array");
    }
    return read_func_param_list(param, basety);
}

static Type *read_declarator_tail(Type *basety, Vector *params ){
    if (next_token('[')){
        return read_declarator_array(basety);
    }
    if (next_token('(')){
        return read_declarator_func(basety, params);
    }
    return basety;
}

static void skip_type_quailfiers(){
    while (next_token(KCONST) || next_token(KVOLATILE) || next_token(KRESTRICT));
}

static Type *read_declarator(char **rname, Type *basety, Vector *params, int ctx){
    if (next_token('(')){
        if (is_type(peek())){
            return read_declarator_func(basety, params);
        }
        Type *stub = make_stub_type();
        Type *t = read_declarator(rname, stub, params, ctx);
        expect(')');
        *stub = *read_declarator_tail(basety,params);
        return t;
    }
    if (next_token('*')){
        skip_type_quailfiers();
        return read_declarator(rname  ,make_ptr_type(basety), params, ctx);
    }
    Token *tok = get();
    if (tok->kind == TIDENT){
        if (ctx == DECL_CAST ){
            errort(tok, "identifier is not expected, but got %s", tok2s(tok));
        }
        *rname = tok->sval;
        return read_declarator_tail(basety, params);
    }
    if (ctx== DECL_BODY || ctx == DECL_PARAM){
        errort(tok, "identifier , ( or * are expected, but got %s", tok2s(tok));
    }
    unget_token(tok);
    return read_declarator_tail(basety, params);
}



