#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include <err.h>
#include "vm.h"

#ifdef DEBUG
#include <stdio.h>
#endif

#define VM_ERR_INVALID_OP_TYPE 1
#define VM_ERR_INVALID_INT_OPERATOR 2
#define VM_ERR_OUT_OF_BOUNDS 3
#define VM_ERR_STACK_OVERFLOW 4

const struct object obj_null = {

	.type  = OBJ_NULL,
};

const struct object obj_true = {

	.type = OBJ_BOOL,
	.value = {.boolean = true},
};
const struct object obj_false= {

	.type = OBJ_BOOL,
	.value = {.boolean  = false},
};

struct vm *vm_new(struct bytecode *bc){

	struct vm *vm = malloc(sizeof *vm);
	vm->stack_pointer = 0;

	for (int i=0; i<STACK_SIZE; i++){
	
		vm->stack[i] = obj_null;
		vm->globals[i] = obj_null;
		vm->constants[i] = obj_null;


	}
	for (int i=0; i<bc->constants->size; i++){
	
		struct object obj = *bc->constants->values[i];

		//copy over string values
	        if(obj.type == OBJ_STRING){
		
			char *dup = malloc(strlen(obj.value.string) + 1);
			if (!dup){
			
				err(EXIT_FAILURE, "out of memory");
			}
			strcpy(dup, obj.value.string);
			obj.value.string = dup;
		}
		vm->constants[i] = obj;
	}

	struct object *fn = make_compiled_function_object(bc->instructions, 0);
	
		vm->frames[0] = frame_new(*fn, 0);
		vm->frame_index = 0;
		free_object(fn);
		return vm;
	
}


struct vm *vm_new_with_globals(struct bytecode *bc, struct object globals[STACK_SIZE]){

	struct vm *vm = vm_new(bc);
	for (int i=0; i<STACK_SIZE; i++){
	
		vm-globals[i] = globals[i];
	}
	return vm;
}


void vm_free(struct vm *vm){

	free(vm);
}

struct frame frame_new(struct object obj, size_t bp){
   
     #ifndef UNSAFE
     assert(obj.type == OBJ_COMPILED_FUNCTION);
     #endif
     struct frame f = {
     
	     .ip = 0,
	     .fn = obj.value.compiled_function,
	     .base_pointer = bp,
     };
     return f;

}


#ifdef OPT_AGGRESSIVE
#define vm_pop_frame(vm) vm->frames[vm->frame_index--]
#define vm_push_frame(vm, f) vm->frames[++vm->frame_index]= f
#else

struct frame vm_pop_frame(struct vm *vm){

	return vm->frames[vm->frame_index--];
}

void vm_push_frame(struct vm *vm , struct frame f){

         #ifndef UNSAFE
	if ((vm->frame_index+1) >= STACK_SIZE){
	
		err(VM_ERR_STACK_OVERFLOW, "frame overflow");
	}
        #endif
	vm->framesp[++vm->frame_index] = f;
}

#endif


#define vm_stack_pop_ignore(vm) vm->stack_pointer--

#ifdef OPT_AGGRESSIVE
#define vm_stack_pop(vm) vm->stack[--vm->stack_pointer]
#define vm_stack_push(vm , obj) vm->stack[vm->stack_pointer++] = obj
#else

struct object vm_stack_pop(struct vm *vm){

      #ifndef UNSAFE
      if (vm->stake_pointer == 0){
      
	      return obj_null;
      }
      #endif
      return vm->stack[--vm->stack_pointer];
}

void vm_stack_push(struct vm *vm, struct object obj){

     #ifndef UNSAFE
     if (vm->stack_pointer >= STACK_SIZE){
     
	     err(VM_ERR_STACK_OVERFLOW, "stack overflow");
	     return;
     }
     #endif

     vm->stack[vm->stack_pointer++] = obj;
}

#endif

int vm_do_binary_integer_operation(struct vm *vm , enum opcode opcode, int left, int right){

	long result;

	switch(opcode){
	
		case OPCODE_ADD:
			result = left+right;
		break;
		case OPCODE_SUBTRACT:
		        result = left-right;
		break;
	        case OPCODE_MULTIPLY:
		        result = left*right;
		break;
		case OPCODE_DIVIDE:
		        result = left/right;
		break;
		default:
		     return VM_ERR_INVALID_INT_OPERATOR;
	        break;
	}

	struct object obj ={
	
		.type = OBJ_INT,
		.value = {.integer = result}
	};
	vm_stack_push(vm ,obj);
	return 0;
}

int vm_do_binary_string_operation(struct vm *vm , enum opcode opcode, char *left, char *right){
   struct object obj = {
   
	   .type = OBJ_STRING,
   };

   //FIXME: Re-use allocation here?

   obj.value.string = malloc(strlen(left) + strlen(right) + 1);

   strcpy(obj.value.string, left);
   strcpy(obj.value.string, right);
   vm_stack_push(vm ,obj);
   return 0;

}


int vm_do_binary_operation(struct vm *vm , enum opcode opcode){

	struct object right = vm_stack_pop(vm);
	struct object left= vm_stack_pop(vm);

	if (left.type != right.type){
	
		return VM_ERR_INVALID_OP_TYPE;
	}

	switch(left.type){
	
		case OBJ_INT: return vm_do_binary_integer_operation(vm ,opcode, left.value.integer, right.value.integer);
		case OBJ_STRING: return vm_do_binary_string_operation(vm, opcode, left.value.string, right.value.string);
		default: return VM_ERR_INVALID_OP_TPYE;
	}
}

int vm_do_integer_comparison(struct vm *vm , enum opcode opcode, int left, int right){

	bool result;

	switch(opcode){
	
		case OPCODE_EQUAL:
			result = left == right;
			break;
		case OPCODE_NOT_EQUAL:
			result = left != right;
			break;
		case OPCODE_LESS_THAN:
			result = left < right;
			break;
		case OPCODE_GREATER_THAN:
			result = left > right;
			break;
		default:
			return VM_ERR_INVALID_OP_TYPE;
		break;

	}
	vm_stack_push(vm , result ? obj_true : obj_false);
	return 0;
}


int vm_do_comparision(struct vm *vm , enum opcode opcode){

	struct object right = vm_stack_pop(vm);
	struct object left = vm_stack_pop(vm);

        #ifndef UNSAFE
	if (left.type != right.type){
	
		return VM_ERR_INVALID_OP_TYPE;
	}
        #endif
	if (left.type == OBJ_INT){
	
		return vm_do_integer_comparison(vm , opcode , left.value.integer, right.value.integer);
	
	}
	bool result;
	switch(opcode){
	
		case OPCODE_EQUAL:
			result = left.value.boolean == right.value.boolean;
			break;

		case OPCODE_NOT_EQUAL:
			result = left.value.boolean != right.value.boolean;
			break;
		default:
			return VM_ERR_INVALID_OP_TYPE;
		break;

	}
	vm_stack_push(vm, result ? obj_true: obj_false);
	return 0;
}

void vm_do_bang_operation(struct vm *vm){

	struct object obj = vm_stack_pop(vm);
	vm_stack_push(vm , obj.type == OBJ_NULL || (obj.type == OBJ_BOOL && obj.value.boolean == false) ? obj_true : obj_false );
}


int vm_do_minus_operation(struct vm *vm){

	struct object obj = vm_stack_pop(vm);
        #ifndef UNSAFE
	if (obj.type != OBJ_INT){
	
		return VM_ERR_INVALID_OP_TYPE;
	}
        #endif

	obj.value.integer = -obj.value.integer;
	vm->stack_push(vm ,obj);
	return 0;
}

int vm_run(struct vm *vm ){

	// values used in main loop
	size_t ip;
	size_t ip_max;
	struct frame *active_frame;
	enum opcode opcode;
	uint8_t *bytes;

	// temp values used in switch cases
	
	int err, idx, pos, num_args;


}










