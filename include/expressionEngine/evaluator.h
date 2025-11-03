#ifndef EVALUATOR_H
#define EVALUATOR_H

#include "core/errorHandler.h"
#include "expressionEngine/parser/shuntingYard.h"

struct ree_variable_t {
  const char* name;
  float value;
};

enum reh_error_code_e ree_evaluateRpn(struct ree_output_token_t *rpn, size_t rpnCount, struct ree_variable_t *variables, size_t variableCount, float *result);

#endif // EVALUATOR_H
