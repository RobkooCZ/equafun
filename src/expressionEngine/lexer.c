#include "expressionEngine/lexer.h"
#include "core/errorHandler.h"
#include "expressionEngine/tokens.h"
#include "core/logger.h"

#include <ctype.h>
#include <stdio.h>
#include <string.h>


void ree_print(struct ree_token_t *tokenVal){
  printf("{%s, %s}\n", ree_tokenToStr(tokenVal->token_type), tokenVal->value);
}

void ree_skipWhitespace(struct ree_data_t *data){
  while (data->currentChar == ' ' || data->currentChar == '\t' || data->currentChar == '\n' || data->currentChar == '\r'){
    ree_advance(data);
  }
}

void ree_advance(struct ree_data_t *data){
  if (data == nullptr){
    logMsg(ERROR, "Data pointer passed to ree_advance is NULL.");
    return;
  }

  if (data->nextPosition >= data->length){
    data->currentChar = ' ';
  }
  else {
    data->currentChar = data->expression[data->nextPosition];
  }

  data->currentPosition = data->nextPosition;
  data->nextPosition++;
  data->charsLexed++;
}

void ree_readNumber(struct ree_data_t *data, char* number){
  if (number == nullptr){
    logMsg(ERROR, "Number pointer passed to ree_readNumber is NULL.");
    return;
  }
  if (data == nullptr){
    logMsg(ERROR, "Data pointer passed to ree_readNumber is NULL.");
    return;
  }

  int start = data->currentPosition;

  while (isdigit(data->currentChar)){
    ree_advance(data);
  }

  // iterator i starts at start, e.g. the position of the number in the expression
  // iterator i starts at 0 for us to fill the number arr
  for (int i = start, j = 0; i < data->currentPosition; ++i, ++j){
    number[j] = data->expression[i];
  }
}

void ree_readIdentifier(struct ree_data_t *data, char* identifier){
  if (identifier == nullptr){
    logMsg(ERROR, "Identifier pointer passed to ree_readIdentifier is NULL.");
    return;
  }
  if (data == nullptr){
    logMsg(ERROR, "Data pointer passed to ree_readIdentifier is NULL.");
    return;
  }

  int start = data->currentPosition;

  while (isalpha(data->currentChar)){
    ree_advance(data);
  }

  // iterator i starts at start, e.g. the position of the number in the expression
  // iterator i starts at 0 for us to fill the number arr
  for (int i = start, j = 0; i < data->currentPosition; ++i, ++j){
    identifier[j] = data->expression[i];
  }
}

enum reh_error_code_e ree_nextToken(struct ree_data_t *data, struct ree_token_t *nextToken){
  if (data == nullptr){
    SET_ERROR_RETURN(ERR_INVALID_POINTER, "Data pointer passed to ree_nextToken is NULL.");
  }
  if (nextToken == nullptr){
    SET_ERROR_RETURN(ERR_INVALID_POINTER, "Next token pointer passed to ree_nextToken is NULL.");
  }

  ree_skipWhitespace(data);

  if (isdigit(data->currentChar)){
    nextToken->token_type = TOKEN_NUMBER;
    ree_readNumber(data, nextToken->value);
    return ERR_SUCCESS; // return early to prevent additional position increment
  }
  else if (isalpha(data->currentChar)){
    nextToken->token_type = TOKEN_IDENTIFIER;
    ree_readIdentifier(data, nextToken->value);
    return ERR_SUCCESS; // return early to prevent additional position increment
  }
  else if (data->currentChar == '+'){
    nextToken->token_type = TOKEN_PLUS;
  }
  else if (data->currentChar == '-'){
    nextToken->token_type = TOKEN_MINUS;
  }
  else if (data->currentChar == '*'){
    nextToken->token_type = TOKEN_MULTIPLY;
  }
  else if (data->currentChar == '/'){
    nextToken->token_type = TOKEN_DIVIDE;
  }
  else if (data->currentChar == '='){
    nextToken->token_type = TOKEN_EQUALS;
  }
  else if (data->currentChar == '^'){
    nextToken->token_type = TOKEN_POWER;
  }
  else if (data->currentChar == '!'){
    nextToken->token_type = TOKEN_FACTORIAL;
  }
  else if (data->currentChar == '('){
    nextToken->token_type = TOKEN_PAREN_OPEN;
  }
  else if (data->currentChar == ')'){
    nextToken->token_type = TOKEN_PAREN_CLOSE;
  }
  else {
    nextToken->token_type = TOKEN_ILLEGAL;
    nextToken->value[0] = (char)data->currentChar;
    return ERR_INPUT_TOKEN_INVALID;
  }

  nextToken->value[0] = (char)data->currentChar;

  ree_advance(data);

  return ERR_SUCCESS;
}

enum reh_error_code_e ree_lexer(char *expression, struct ree_token_t *tokens){
  if (expression == nullptr){
    SET_ERROR_RETURN(ERR_INVALID_POINTER, "Expression passed to ree_lexer is NULL.");
  }
  if (tokens == nullptr){
    SET_ERROR_RETURN(ERR_INVALID_POINTER, "Tokens array passed to ree_lexer is NULL.");
  }

  struct ree_data_t data;

  // fill the data struct
  data.currentPosition = 0;
  data.nextPosition = 1;
  data.currentChar = expression[0];
  data.expression = expression;
  data.length = (int)strlen(data.expression);

  int i = 0;
  // loop through the entire expression
  while (data.currentPosition != data.length){
    // make sure to set all the chars of the string in the struct to empty char to prevent gibberish
    struct ree_token_t token = {0, {' '}};

    CHECK_ERROR_CTX(ree_nextToken(&data, &token), "Invalid character in lexed expression.");
    tokens[i] = token;
    i++;
  }

  return ERR_SUCCESS;
}

enum reh_error_code_e ree_countTokens(char* expression, int *tokenCount){
  if (expression == nullptr){
    SET_ERROR_RETURN(ERR_INVALID_POINTER, "Expression passed to ree_countTokens is NULL.");
  }
  if (tokenCount == nullptr){
    SET_ERROR_RETURN(ERR_INVALID_POINTER, "Pointer to tokenCount in ree_countTokens is NULL.");
  }

  struct ree_data_t data;

  // fill the data struct
  data.currentPosition = 0;
  data.nextPosition = 1;
  data.charsLexed = 0;
  data.currentChar = expression[0];
  data.expression = expression;
  data.length = (int)strlen(data.expression);

  int i = 0;
  // loop through the entire expression
  while (data.currentPosition != data.length){
    // make sure to set all the chars of the string in the struct to empty char to prevent gibberish
    struct ree_token_t token = {0, {' '}};

    CHECK_ERROR_CTX(ree_nextToken(&data, &token), "Invalid character in lexed expression: %c", data.currentChar);
    i++;
  }

  *tokenCount = i;

  return ERR_SUCCESS;
}
