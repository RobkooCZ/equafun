#ifndef POSTFIX_PARSER_H
#define POSTFIX_PARSER_H

#include "core/errorHandler.h"
#include "expressionEngine/lexer.h"
#include "expressionEngine/tokens.h"

enum ree_output_type_e {
  OUTPUT_NUMBER = 0,
  OUTPUT_OPERATOR = 1,
  OUTPUT_FUNCTION = 2,
};

struct ree_output_token_t {
  enum ree_output_type_e type;
  int arity;
  char symbol[MAX_BUF_SIZE];
  float value;
};

extern const char* validFunctions[];
extern const int functionArrLength;

const char* ree_outputTokenToStr(enum ree_output_type_e outputToken);

enum reh_error_code_e ree_operatorPrecedence(enum ree_token_type_e tokenType, int *precedence);
int ree_determineArity(enum ree_token_type_e tokenType);
enum reh_error_code_e ree_markUnaryOperators(struct ree_token_t *tokens, const int tokenCount);

// rpn - reverse polish notation
enum reh_error_code_e ree_parseToPostfix(struct ree_token_t *tokens, const int tokenCount, struct ree_output_token_t *outputQueue, int *outputCount);

#endif // POSTFIX_PARSER_H
