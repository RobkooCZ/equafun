#include "expressionEngine/lexer.h"
#include "core/errorHandler.h"
#include "expressionEngine/tokens.h"

#include <ctype.h>
#include <stdio.h>
#include <string.h>

enum reh_error_code_e ree_skipWhitespace(struct ree_data_t *data){
  while (data->currentChar == ' ' || data->currentChar == '\t' || data->currentChar == '\n' || data->currentChar == '\r'){
    ree_advance(data);
  }

  return ERR_SUCCESS;
}
enum reh_error_code_e ree_advance(struct ree_data_t *data){
  if (data->nextPosition >= data->length){
    data->currentChar = ' ';
  }
  else {
    data->currentChar = data->expression[data->nextPosition];
  }

  data->currentPosition = data->nextPosition;
  data->nextPosition++;

  return ERR_SUCCESS;
}

enum reh_error_code_e ree_readNumber(struct ree_data_t *data, char* number){
  int start = data->currentPosition;

  while (isdigit(data->currentChar)){
    ree_advance(data);
  }

  // iterator i starts at start, e.g. the position of the number in the expression
  // iterator i starts at 0 for us to fill the number arr
  for (int i = start, j = 0; i < data->currentPosition; ++i, ++j){
    number[j] = data->expression[i];
  }

  return ERR_SUCCESS;
}

enum reh_error_code_e ree_lexer(char *expression, struct ree_token_t *tokens){
  struct ree_data_t data;

  // fill the data struct
  data.nextPosition = 0;
  data.currentPosition = 0;
  data.currentChar = expression[0];
  data.expression = expression;
  data.length = strlen(data.expression);

  // loop through the entire expression
  while (data.currentPosition != data.length){
  // make sure to set all the chars of the string in the struct to empty char to prevent gibberish
    struct ree_token_t testToken = {0, {' '}};

    ree_nextToken(&data, &testToken);
    ree_print(&testToken);
  }

  return ERR_SUCCESS;
}

enum reh_error_code_e ree_nextToken(struct ree_data_t *data, struct ree_token_t *nextToken){
  ree_skipWhitespace(data);

  if (isdigit(data->currentChar)){
    nextToken->token_type = TOKEN_NUMBER;
    ree_readNumber(data, nextToken->value);
    return ERR_SUCCESS; // return early to prevent additional position increment 
  }
  else if (data->currentChar == '+'){
    nextToken->token_type = TOKEN_PLUS;
    nextToken->value[0] = data->currentChar;
  }
  else if (data->currentChar == '-'){
    nextToken->token_type = TOKEN_MINUS;
    nextToken->value[0] = data->currentChar;
  }
  else if (data->currentChar == '*'){
    nextToken->token_type = TOKEN_MULTIPLY;
    nextToken->value[0] = data->currentChar;
  }
  else if (data->currentChar == '/'){
    nextToken->token_type = TOKEN_DIVIDE;
    nextToken->value[0] = data->currentChar;
  }
  else if (data->currentChar == '='){
    nextToken->token_type = TOKEN_EQUALS;
    nextToken->value[0] = data->currentChar;
  }
  else {
    nextToken->token_type = TOKEN_ILLEGAL;
  }

  ree_advance(data);

  return ERR_SUCCESS;
}

void ree_print(struct ree_token_t *tokenVal){
  printf("{%s, %s} ", ree_tokenToStr(tokenVal->token_type), tokenVal->value);
}
