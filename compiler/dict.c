#include "compiler.h"
#include <stdlib.h>


Dict *make_dict(){
    Dict *r = malloc(sizeof(Dict));
    r->map = make_map();
    r->key = make_vector();
    return r;
}

void *dict_get(Dict *dict,char *key){
    return map_get(dict->map, key);
}

void dict_put(Dict *dict, char *key, void *val){
    map_put(dict->map, key, val);
    vec_push(dict->key,key);
}
Vector *dict_keys(Dict *dict){
    return dict->key;
}