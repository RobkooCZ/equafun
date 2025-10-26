#ifndef LEXER_H
#define LEXER_H

#include "tokens.h"
#include "core/errorHandler.h"
#define MAX_BUF_SIZE 256

struct ree_token_t {
  enum ree_token_type_e token_type;
  char value[MAX_BUF_SIZE];
};

struct ree_data_t {
  int currentPosition;
  int nextPosition;
  int currentChar;
  int length;
  char* expression;
};

enum reh_error_code_e ree_nextToken(struct ree_data_t *data, struct ree_token_t *nextToken);
enum reh_error_code_e ree_advance(struct ree_data_t *data);
enum reh_error_code_e ree_readNumber(struct ree_data_t *data, char* number);
enum reh_error_code_e ree_skipWhitespace(struct ree_data_t *data);

enum reh_error_code_e ree_lexer(char* expression, struct ree_token_t *tokens);
// helper function to print the struct
void ree_print(struct ree_token_t *tokenVal);

#endif // LEXER_H
