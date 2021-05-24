#include "compiler.h"
#include <string.h>
#include <stdlib.h>

Set *set_add(Set *s, char *v){
    Set *r = malloc(sizeof(Set));
    r->next = s;
    r->v = v;
    return r;
}

bool set_has(Set *s, char *v){
    for (;s; s=s->next){
        if (!strcmp(s->v,v)){
            return true;
        }
    }
    return false;
}


Set *set_union(Set *a, Set *b){
    Set *r = b;
    for (; a; a=a->next){
        if (!set_has(b,a->v)){
            r = set_add(r, a->v);
        }
    }
    return r;
}


Set *set_intersection(Set *a, Set *b){
    Set *r = NULL;
    for (;a;a=a->next){
        if (set_has(b, a->v)){
            r =set_add(r, a->v);
        }
    }
    return r;
}
