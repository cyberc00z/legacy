#include <stdlib.h>
#include <string.h>
#include <err.h>
#include "symbol_table.h"

struct hashmap *hashmap_new(){

	struct hashmap *hm = malloc(sizeof *hm);
	if (!hm) err(EXIT_FAILURE, "out of memory");

	for (int i=0; i<26; i++){
	
		hm->table[i] = NULL;
	}

	return hm;
}

void hashmap_insert(struct hashmap *hm, char *key, void *item){

	int pos = hash(key);
	struct hashmap_node *head = hm->table[pos];
	struct hashmap_node *node = head;

	// walk list to find existing value with that key
	
	while (node){
	
		if (strcmp(node->key, key) == 0){
		
			// free(item);
			node->value = item;
			return;
		}
		node = node->next;
	}

	// insert new node at start of list
	node = malloc(sizeof *node);
	node->key = key;
	node->value = item;
	node->next = head;
	hm->table[pos] = node;
}


void *hashmap_get(struct hashmap *hm, char *key){

	int pos = hash(key);
	struct hashmap_node *node = hm->table[pos];
	while (node){
	
		if (strcmp(node->key, key) == 0){
		
			return node->value;
		}
		node  = node->next;
	}
	return NULL;
}

void hashmap_free(struct hashmap *hm){

	struct hashmap_node *node;
	struct hashmap_node *next;

	for (int i=0;i<26; i++){
	
		node = hm->table[i];
		while(node){
		
			next = node->next;
			free(node->value);

		}
	}
	free(hm);
}



struct symbol_table *symbol_table_new(){

	struct symbol_table *t = malloc(sizeof *t);
	if (!t) err(EXIT_FAILURE, "out of memory");

	t->size = 0;
	t->store = hashmap_new();
	t->outer = NULL;
	return t;
}


struct symbol_table *symbol_table_new_enclosed(struct symbol_table *outer){

	struct symbol_table *t= symbol_table_new();
	t->outer = outer;
	return t;
}

struct symbol *symbol_table_define(struct symbol_table *t, char *name){

	struct symbol *s = malloc(sizeof *s);
	if (!s) err(EXIT_FAILURE, "out of memory");

	// TODO: Copy name?
	// Should only be required if we free the original program string before evaluting bytecodes
	s->name  = name;
	s->scope  = t->outer ? SCOPE_LOCAL : SCOPE_GLOBAL;
	s->index = t->size;

	// insert into store
	//
	hashmap_insert(t->store, name, s);
	t->size++;
	return s;
}


struct symbol *symbol_table_define_function(struct symbol_table *t, char *name){


	struct symbol *s =  malloc(sizeof *s);
	if (!s) err(EXIT_FAILURE, "out of memory");

	// TODO: Copy name?
	// Should only be required if we free the original program string program before evaluting bytecode
	s->name = name;
	s->scope = SCOPE_FUNCTION;
	s->index = 0;
	hashmap_insert(t->store, name, s);
	return s;
}


struct symbol *symbol_table_resolve(struct symbol_table *t, char *name){

	void *r = hashmap_get(t->store, name);
	if (r== NULL){
	     if (t->outer){
	          return symbol_table_resolve(t->outer, name);
	     }
	     return NULL;

	}
	return (struct symbol *) r;
}

void symbol_table_free(struct symbol_table *t){

	hashmap_free(t->store);
	free(t);
}






