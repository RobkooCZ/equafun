#include "expressionEngine/parser/functionParser.h"
#include "core/errorHandler.h"
#include "expressionEngine/functionManager.h"
#include "expressionEngine/lexer.h"
#include "expressionEngine/parser/shuntingYard.h"
#include "expressionEngine/tokens.h"
#include "math/Vec3.h"
#include <string.h>
#include <stdlib.h>

enum reh_error_code_e ree_parseFunctionDefinition(char *definition, struct ree_function_t *function){
  int tokenCount = 0;
  CHECK_ERROR_CTX(ree_countTokens(definition, &tokenCount), "Failed to count tokens.");

  struct ree_token_t tokens[tokenCount];
  CHECK_ERROR_CTX(ree_lexer(definition, tokens), "Failed to perform lexical analysis.");

  // tokens[0].token_type MUST be an identifier (for example: f)
  // tokens[1].token_type MUST be an open parenthesis '('
  // tokens[2].token_type MUST be an identifier (such as 'x') but CANNOT have the same value as tokens[0]
  // tokens[3].token_type MUST be a  close parenthesis ')'
  // tokens[4].token_type MUST be an equals sign '='

  // check for function identifier and make sure its not longer than the allowed size (to prevent buffer overflow)
  if (tokens[0].token_type != TOKEN_IDENTIFIER){
    SET_ERROR_RETURN(ERR_INVALID_INPUT, "Function identifier incorrect: %s instead of %s with value: %s", ree_tokenToStr(tokens[0].token_type), ree_tokenToStr(TOKEN_IDENTIFIER), tokens[0].value);
  }
  else if (strlen(tokens[0].value) > (unsigned long)MAX_FN_NAME_LEN){
    SET_ERROR_RETURN(ERR_INVALID_INPUT, "Function identifier too long: %lu chars. Max length: %d chars.", strlen(tokens[0].value), MAX_FN_NAME_LEN);
  }

  // check for left parentheses '('
  if (tokens[1].token_type != TOKEN_PAREN_OPEN){
    SET_ERROR_RETURN(ERR_INVALID_INPUT, "Function open parenthesis incorrect: %s instead of %s with value: %s", ree_tokenToStr(tokens[1].token_type), ree_tokenToStr(TOKEN_PAREN_OPEN), tokens[1].value);
  }

  // check for parameter, make sure it doesn't match the function identifier and make sure its not longer than allowed (to prevent buffer overflow)
  if (tokens[2].token_type != TOKEN_IDENTIFIER){
    SET_ERROR_RETURN(ERR_INVALID_INPUT, "Function parameter incorrect: %s instead of %s with value: %s", ree_tokenToStr(tokens[2].token_type), ree_tokenToStr(TOKEN_IDENTIFIER), tokens[2].value);
  }
  else if (strcmp(tokens[0].value, tokens[2].value) == 0){
    SET_ERROR_RETURN(ERR_INVALID_INPUT, "Function identifier cannot be the same as the parameter.");
  }
  else if (strlen(tokens[2].value) > (unsigned long)MAX_PARAM_NAME_LEN){
    SET_ERROR_RETURN(ERR_INVALID_INPUT, "Function parameter too long: %lu chars. Max length: %d chars.", strlen(tokens[2].value), MAX_PARAM_NAME_LEN);
  }

  // check for right parenthesis ')'
  if (tokens[3].token_type != TOKEN_PAREN_CLOSE){
    SET_ERROR_RETURN(ERR_INVALID_INPUT, "Function close parenthesis incorrect: %s instead of %s with value: %s", ree_tokenToStr(tokens[3].token_type), ree_tokenToStr(TOKEN_PAREN_CLOSE), tokens[3].value);
  }

  // check for equals sign '='
  if (tokens[4].token_type != TOKEN_EQUALS){
    SET_ERROR_RETURN(ERR_INVALID_INPUT, "Function equals sign incorrect: %s instead of %s with value: %s", ree_tokenToStr(tokens[4].token_type), ree_tokenToStr(TOKEN_EQUALS), tokens[4].value);
  }

  // all valid, proceed to fill the function struct
  strcpy(function->name, tokens[0].value);
  strcpy(function->parameter, tokens[2].value);

  // fill tokens array with all tokens besides the ones defining the function name and variable (as the parser doesn't handle 'f(x) =')
  // allocate token array
  function->tokens = malloc(tokenCount * sizeof(struct ree_token_t));
  if (function->tokens == NULL){
    free(function->tokens);
    SET_ERROR_RETURN(ERR_OUT_OF_MEMORY, "Failed to allocate memory for function tokens.");
  }

  int j = 0;
  for (int i = 5; i < tokenCount; i++, j++){
    function->tokens[j] = tokens[i];
  }

  function->tokenCount = j;

  // allocate RPN array
  function->rpn = malloc(function->tokenCount * sizeof(struct ree_output_token_t));
  if (function->rpn == NULL){
    free(function->tokens);
    SET_ERROR_RETURN(ERR_OUT_OF_MEMORY, "Failed to allocate memory for RPN array.");
  }

  // mark unary operators, parse and assign the result
  CHECK_ERROR_CTX(ree_markUnaryOperators(function->tokens, function->tokenCount), "Failed to mark unary operators.");
  CHECK_ERROR_CTX(ree_parseToPostfix(function->tokens, function->tokenCount, function->rpn, &function->rpnCount), "Failed to parse tokens into RPN.");

  // rendering data
  function->isVisible = true;
  function->color = (struct rm_vec3_t){255.0f, 255.0f, 255.0f}; // default white

  return ERR_SUCCESS;
}
