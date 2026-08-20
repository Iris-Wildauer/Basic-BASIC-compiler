#ifndef EVAL_H
#define EVAL_H

#include "ast.h"

int execExpr(Expr *e);

void execPrin(const Prin *prin);

void execProgram(const Program *prog);

#endif
