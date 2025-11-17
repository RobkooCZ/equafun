/**
  ree - Robkoo's Expression Engine
*/
#ifndef LEXER_H
#define LEXER_H

#include "tokens.h"
#include "core/errorHandler.h"
#define MAX_BUF_SIZE 256

struct ree_token_t {
  enum ree_token_type_e token_type; /**< type of the token */
  char value[MAX_BUF_SIZE];         /**< string representation of the token */
};

struct ree_data_t {
  int currentPosition; /**< current position in the expression */
  int nextPosition;    /**< next position in the expression */
  int charsLexed;      /**< number of characters lexed so far */
  int currentChar;     /**< current character being processed */
  int length;          /**< length of the expression */
  char* expression;    /**< the expression being lexed */
};

/**
  @brief Prints a token's details (for debugging)
*/
void ree_Print(struct ree_token_t *tokenVal);

/**
  @brief Skips whitespace characters in the input data
*/
void ree_SkipWhitespace(struct ree_data_t *data);

/**
  @brief Advances the current position in the input data
*/
void ree_Advance(struct ree_data_t *data);

/**
  @brief Reads a number from the input data
*/
void ree_ReadNumber(struct ree_data_t *data, char* number);

/**
  @brief Reads an identifier from the input data
*/
void ree_ReadIdentifier(struct ree_data_t *data, char* identifier);

/**
  @brief Retrieves the next token from the input data
*/
enum reh_error_code_e ree_NextToken(struct ree_data_t *data, struct ree_token_t *nextToken);

/**
  @brief Counts the number of tokens in the expression
*/
enum reh_error_code_e ree_CountTokens(char* expression, int *tokenCount);

/**
  @brief Lexes the expression into tokens
*/
enum reh_error_code_e ree_Lexer(char* expression, struct ree_token_t *tokens);

#endif // LEXER_H
