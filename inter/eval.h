#ifndef EVAL_H
#define EVAL_H

#include "object.h"
#include "parser.h"
#include "env,h"

struct object *eval_program(struct program *prog, struct environment *env);

#endif
