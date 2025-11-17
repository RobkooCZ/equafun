/**
  ree - Robkoo's Expression Engine
*/

#ifndef TOKENS_H
#define TOKENS_H

enum ree_token_type_e {
  TOKEN_NUMBER, TOKEN_IDENTIFIER, TOKEN_FUNCTION,
  TOKEN_OPERATOR, TOKEN_PAREN_OPEN, TOKEN_PAREN_CLOSE,
  TOKEN_COMMA, TOKEN_VARIABLE, TOKEN_RL_END,
  TOKEN_PLUS, TOKEN_MINUS, TOKEN_MULTIPLY, TOKEN_DIVIDE,
  TOKEN_POWER, TOKEN_FACTORIAL, TOKEN_UNARY_MINUS, TOKEN_UNARY_PLUS,
  TOKEN_EQUALS, TOKEN_ILLEGAL, TOKEN_EOF
};

/**
  @brief Converts a token type to its string representation
*/
const char* ree_TokenToStr(enum ree_token_type_e tokenType);

#endif
