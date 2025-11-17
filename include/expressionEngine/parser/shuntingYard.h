/**
  ree - Robkoo's Expression Engine
*/

#ifndef POSTFIX_PARSER_H
#define POSTFIX_PARSER_H

#include "core/errorHandler.h"
#include "expressionEngine/lexer.h"
#include "expressionEngine/tokens.h"

#define REE_MAX_TOKENS 256

enum ree_output_type_e {
  OUTPUT_NUMBER = 0,
  OUTPUT_OPERATOR = 1,
  OUTPUT_FUNCTION = 2,
};

struct ree_output_token_t {
  enum ree_output_type_e type;  /**< Type of the output token */
  int arity;                    /**< Arity of the operator or function */
  char symbol[MAX_BUF_SIZE];    /**< Symbol representation */
  float value;                  /**< Numerical value for number tokens */
};

extern const char* validFunctions[];
extern const int functionArrLength;

/**
  @brief Converts an output token type to its string representation
*/
const char* ree_OutputTokenToStr(enum ree_output_type_e outputToken);

/**
  @brief Gets the precedence of an operator token
*/
enum reh_error_code_e ree_OperatorPrecedence(enum ree_token_type_e tokenType, int *precedence);

/**
  @brief Determines the arity of an operator token
*/
int ree_DetermineArity(enum ree_token_type_e tokenType);

/**
  @brief Marks unary operators in a token array
*/
enum reh_error_code_e ree_MarkUnaryOperators(struct ree_token_t *tokens, const int tokenCount);

/**
  @brief Parses an array of tokens into postfix notation using the Shunting Yard algorithm
*/
enum reh_error_code_e ree_ParseToPostfix(struct ree_token_t *tokens, const int tokenCount, struct ree_output_token_t *outputQueue, int *outputCount);

#endif // POSTFIX_PARSER_H
