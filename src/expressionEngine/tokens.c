#include "expressionEngine/tokens.h"

const char* ree_TokenToStr(enum ree_token_type_e tokenType){
  switch (tokenType){
    case TOKEN_NUMBER:       return "TOKEN_NUMBER";
    case TOKEN_IDENTIFIER:   return "TOKEN_IDENTIFIER";
    case TOKEN_FUNCTION:     return "TOKEN_FUNCTION";
    case TOKEN_OPERATOR:     return "TOKEN_OPERATOR";
    case TOKEN_PAREN_OPEN:   return "TOKEN_PAREN_OPEN";
    case TOKEN_PAREN_CLOSE:  return "TOKEN_PAREN_CLOSE";
    case TOKEN_COMMA:        return "TOKEN_COMMA";
    case TOKEN_VARIABLE:     return "TOKEN_VARIABLE";
    case TOKEN_RL_END:          return "TOKEN_RL_END";
    case TOKEN_PLUS:         return "TOKEN_PLUS";
    case TOKEN_MINUS:        return "TOKEN_MINUS";
    case TOKEN_MULTIPLY:     return "TOKEN_MULTIPLY";
    case TOKEN_DIVIDE:       return "TOKEN_DIVIDE";
    case TOKEN_POWER:        return "TOKEN_POWER";
    case TOKEN_FACTORIAL:    return "TOKEN_FACTORIAL";
    case TOKEN_UNARY_MINUS:  return "TOKEN_UNARY_MINUS";
    case TOKEN_UNARY_PLUS:   return "TOKEN_UNARY_PLUS";
    case TOKEN_EQUALS:       return "TOKEN_EQUALS";
    case TOKEN_ILLEGAL:      return "TOKEN_ILLEGAL";
    case TOKEN_EOF:          return "TOKEN_EOF";
    default:                 return "Unknown Token";
  }
}
