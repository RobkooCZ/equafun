#include "expressionEngine/parser/functionParser.h"
#include "core/errorHandler.h"
#include "expressionEngine/functionManager.h"
#include "expressionEngine/lexer.h"
#include "expressionEngine/parser/shuntingYard.h"
#include "expressionEngine/tokens.h"
#include "math/Vec3.h"
#include "utils/utilities.h"
#include <string.h>
#include <stdlib.h>
#include "core/logger.h"


enum reh_error_code_e ree_testLexerParser(void){
  // lexer, parser testing
  char* expression = "f(x) = (-3 + 4x) * 4!^2";
  printf("Expression input: %s\n", expression);

  struct ree_function_t function;

  CHECK_ERROR_CTX(ree_parseFunction(expression, &function, &functionColorArray[0]), "Failed to parse function definition.");

  // print function metadata
  printf("Function Name: %s\n", function.name);
  printf("Parameter: %s\n", function.parameter);
  printf("Token Count: %d\n", function.tokenCount);
  printf("RPN Count: %d\n", function.rpnCount);
  printf("Is Visible: %s\n", function.isVisible ? "true" : "false");
  printf("Color: (%.2f, %.2f, %.2f)\n", (double)function.color.x, (double)function.color.y, (double)function.color.z);
  
  // Print tokens
  printf("\nTokens:\n");
  for (int i = 0; i < function.tokenCount; i++) {
    printf("\tToken %d: type=%s, value=[%s]\n",
           i,
           ree_tokenToStr(function.tokens[i].token_type),
           function.tokens[i].value);
  }
 
  // Print RPN
  printf("\nRPN:\n");
  for (int i = 0; i < function.rpnCount; i++) {
    printf("\tRPN %d: type=%s, value=[%s]\n",
           i,
           ree_outputTokenToStr(function.rpn[i].type),
           function.rpn[i].symbol);
  }

  printf("\n");

  // memory cleanup
  free(function.rpn);
  free(function.tokens);

  return ERR_SUCCESS;
}

enum reh_error_code_e ree_implicitMultiplication(struct ree_token_t **tokens, int *tokenCount, int *tokenCapacity){
  // walk the array of tokens
  for (int i = 0; i < *tokenCount - 1; i++){
    // set bools based on if we get one of the required token types
    bool leftFactor =  (*tokens)[i].token_type == TOKEN_NUMBER         ||
                       (*tokens)[i].token_type == TOKEN_IDENTIFIER     ||
                       (*tokens)[i].token_type == TOKEN_PAREN_CLOSE;

    bool rightFactor = (*tokens)[i+1].token_type == TOKEN_NUMBER         ||
                       (*tokens)[i+1].token_type == TOKEN_IDENTIFIER     ||
                       (*tokens)[i+1].token_type == TOKEN_FUNCTION       ||
                       (*tokens)[i+1].token_type == TOKEN_PAREN_OPEN;

    bool leftIsFunction = ((*tokens)[i].token_type == TOKEN_IDENTIFIER) &&
                      rgu_isStrInArray(validFunctions, functionArrLength, (*tokens)[i].value);

    // bail out early if the left factor is a known function
    // if we kept it, sin(x) would turn into sin * (x) and thus break the parser
    if (leftIsFunction){
      continue;
    }

    // dont do anything unless both factors are true
    if ((!leftFactor || !rightFactor)){
      continue;
    }
    if (*tokenCount + 1 > *tokenCapacity){
      int newCapacity = (*tokenCapacity == 0) ? 8 : (*tokenCapacity * 2);
      while (*tokenCount + 1 > newCapacity){
        newCapacity *= 2;
      }

      struct ree_token_t *tmp = realloc(*tokens, (long unsigned int)newCapacity * sizeof **tokens);
      if (tmp == nullptr){
        SET_ERROR_RETURN(ERR_OUT_OF_MEMORY, "Failed to expand token buffer in ree_implicitMultiplication.");
      }

      *tokens = tmp;
      *tokenCapacity = newCapacity;
    }
    // shift the array to the right by one slot to have space to insert the multiplication token
    memmove(&(*tokens)[i+2],
            &(*tokens)[i+1],
            (long unsigned int)(*tokenCount - (i + 1)) * sizeof **tokens);

    (*tokens)[i+1].token_type = TOKEN_MULTIPLY;
    strcpy((*tokens)[i+1].value, "*");
    (*tokenCount)++;
    i++;
  }

  return ERR_SUCCESS;
}

enum reh_error_code_e ree_parseFunction(char *definition, struct ree_function_t *function, struct rm_vec3_t *functionColor){
  int tokenCount = 0;
  CHECK_ERROR_CTX(ree_countTokens(definition, &tokenCount), "Failed to count tokens.");

  struct ree_token_t tokens[tokenCount];
  CHECK_ERROR_CTX(ree_lexer(definition, tokens), "Failed to perform lexical analysis.");

  // ---- f(x) = ... ----
  // tokens[0].token_type MUST be an identifier (for example: f)
  // tokens[1].token_type MUST be an open parenthesis '('
  // tokens[2].token_type MUST be an identifier (such as 'x') but CANNOT have the same value as tokens[0]
  // tokens[3].token_type MUST be a  close parenthesis ')'
  // tokens[4].token_type MUST be an equals sign '='

  // ---- y = ...   ----
  // tokens[0].token_type MUST be an identifier AND its value must be 'y'
  // tokens[1].token_type MUST be an equals sign '='
  bool isYFunctionDefinition = false;

  // check for function identifier and make sure its not longer than the allowed size (to prevent buffer overflow)
  if (tokens[0].token_type != TOKEN_IDENTIFIER){
    SET_ERROR_RETURN(ERR_INVALID_INPUT, "Function identifier incorrect: %s instead of %s with value: %s", ree_tokenToStr(tokens[0].token_type), ree_tokenToStr(TOKEN_IDENTIFIER), tokens[0].value);
  }
  else if (strlen(tokens[0].value) > (unsigned long)MAX_FN_NAME_LEN){
    SET_ERROR_RETURN(ERR_INVALID_INPUT, "Function identifier too long: %zu chars. Max length: %d chars.", strlen(tokens[0].value), MAX_FN_NAME_LEN);
  }
  else if (strcmp(tokens[0].value, "y") == 0){
    isYFunctionDefinition = true;
  }

  // check for left parentheses '(' or '='
  if (tokens[1].token_type != TOKEN_EQUALS && isYFunctionDefinition == true){
    SET_ERROR_RETURN(ERR_INVALID_INPUT, "Function equals (for function definition such as y = ...) incorrect: %s instead of %s with value: %s", ree_tokenToStr(tokens[1].token_type), ree_tokenToStr(TOKEN_PAREN_OPEN), tokens[1].value);
  }
  else if (tokens[1].token_type != TOKEN_PAREN_OPEN && isYFunctionDefinition == false){
    SET_ERROR_RETURN(ERR_INVALID_INPUT, "Function open parenthesis incorrect: %s instead of %s with value: %s", ree_tokenToStr(tokens[1].token_type), ree_tokenToStr(TOKEN_PAREN_OPEN), tokens[1].value);
  }

  // check for parameter, make sure it doesn't match the function identifier and make sure its not longer than allowed (to prevent buffer overflow) only in the case the 'f(x)' style function definition was inputted
  if (tokens[2].token_type != TOKEN_IDENTIFIER && isYFunctionDefinition == false){
    SET_ERROR_RETURN(ERR_INVALID_INPUT, "Function parameter incorrect: %s instead of %s with value: %s", ree_tokenToStr(tokens[2].token_type), ree_tokenToStr(TOKEN_IDENTIFIER), tokens[2].value);
  }
  else if (strcmp(tokens[0].value, tokens[2].value) == 0 && isYFunctionDefinition == false){
    SET_ERROR_RETURN(ERR_INVALID_INPUT, "Function identifier cannot be the same as the parameter.");
  }
  else if (strlen(tokens[2].value) > (unsigned long)MAX_PARAM_NAME_LEN && isYFunctionDefinition == false){
    SET_ERROR_RETURN(ERR_INVALID_INPUT, "Function parameter too long: %zu chars. Max length: %d chars.", strlen(tokens[2].value), MAX_PARAM_NAME_LEN);
  }else 

  // check for right parenthesis ')' only in the case the 'f(x)' style function definition was inputted
  if (tokens[3].token_type != TOKEN_PAREN_CLOSE && isYFunctionDefinition == false){
    SET_ERROR_RETURN(ERR_INVALID_INPUT, "Function close parenthesis incorrect: %s instead of %s with value: %s", ree_tokenToStr(tokens[3].token_type), ree_tokenToStr(TOKEN_PAREN_CLOSE), tokens[3].value);
  }

  // check for equals sign '=' only in the case the 'f(x)' style function definition was inputted
  if (tokens[4].token_type != TOKEN_EQUALS && isYFunctionDefinition == false){
    SET_ERROR_RETURN(ERR_INVALID_INPUT, "Function equals sign incorrect: %s instead of %s with value: %s", ree_tokenToStr(tokens[4].token_type), ree_tokenToStr(TOKEN_EQUALS), tokens[4].value);
  }

  // all valid, proceed to fill the function struct
  strcpy(function->name, tokens[0].value);
  (isYFunctionDefinition == true) ? strcpy(function->parameter, "x") : strcpy(function->parameter, tokens[2].value);

  // fill tokens array with all tokens besides the ones defining the function name and variable (as the parser doesn't handle 'f(x) =' or 'y =')
  // allocate token array
  function->tokens = malloc((long unsigned int)tokenCount * sizeof(struct ree_token_t));
  if (function->tokens == NULL){
    free(function->tokens);
    SET_ERROR_RETURN(ERR_OUT_OF_MEMORY, "Failed to allocate memory for function tokens.");
  }

  int j = 0;
  int fnDefTokenCount = (isYFunctionDefinition == true) ? 2 : 5;
  for (int i = fnDefTokenCount; i < tokenCount; i++, j++){
    function->tokens[j] = tokens[i];
  }

  function->tokenCount = j;
  function->tokenCapacity = j;

  // support for implicit multiplication
  ree_implicitMultiplication(&function->tokens, &function->tokenCount, &function->tokenCapacity);

  // allocate RPN array
  function->rpn = malloc((long unsigned int)function->tokenCount * sizeof(struct ree_output_token_t));
  if (function->rpn == NULL){
    free(function->tokens);
    SET_ERROR_RETURN(ERR_OUT_OF_MEMORY, "Failed to allocate memory for RPN array.");
  }

  // mark unary operators, parse and assign the result
  CHECK_ERROR_CTX(ree_markUnaryOperators(function->tokens, function->tokenCount), "Failed to mark unary operators.");
  CHECK_ERROR_CTX(ree_parseToPostfix(function->tokens, function->tokenCount, function->rpn, &function->rpnCount), "Failed to parse tokens into RPN.");

  // rendering data
  function->isVisible = true;
  function->color = *functionColor;

  return ERR_SUCCESS;
}
