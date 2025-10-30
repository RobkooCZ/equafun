#include "expressionEngine/parser/shuntingYard.h"
#include "core/errorHandler.h"
#include "expressionEngine/lexer.h"
#include "expressionEngine/tokens.h"
#include "utils/utilities.h"
#include <stdlib.h>
#include <string.h>

// visible/usable only in this file
static const char* validFunctions[] = {"sin", "cos", "tan", "sqrt", "abs", "ln", "log"};
static const int functionArrLength = sizeof(validFunctions) / sizeof(validFunctions[0]);

const char* ree_outputTokenToStr(enum ree_output_type_e outputToken){
  switch (outputToken){
    case OUTPUT_NUMBER:   return "OUTPUT_NUMBER";
    case OUTPUT_OPERATOR: return "OUTPUT_OPERATOR";
    case OUTPUT_FUNCTION: return "OUTPUT_FUNCTION";
    default: return "Unknown Output Token";
  }
}

enum reh_error_code_e ree_operatorPrecedence(enum ree_token_type_e tokenType, int *precedence){
  switch (tokenType){
    case TOKEN_PLUS:
    case TOKEN_MINUS:
      *precedence = 1;
      break;
    case TOKEN_MULTIPLY:
    case TOKEN_DIVIDE:
      *precedence = 2;
      break;
    case TOKEN_POWER:
      *precedence = 3;
      break;
    case TOKEN_UNARY_MINUS:
    case TOKEN_UNARY_PLUS:
      *precedence = 4;
      break;
    case TOKEN_FACTORIAL:
      *precedence = 5;
      break;
    case TOKEN_FUNCTION:
      *precedence = 6;
      break;
    default:
      return ERR_INVALID_OPERATOR;
  }

  return ERR_SUCCESS;
}

int ree_determineArity(enum ree_token_type_e tokenType){
  switch (tokenType){
    case TOKEN_UNARY_PLUS:
    case TOKEN_UNARY_MINUS:
    case TOKEN_FACTORIAL:
    case TOKEN_IDENTIFIER:
      return 1;

    case TOKEN_PLUS:
    case TOKEN_MINUS:
    case TOKEN_MULTIPLY:
    case TOKEN_DIVIDE:
    case TOKEN_POWER:
      return 2;

    default:
      return 0;
  }
}

enum reh_error_code_e ree_markUnaryOperators(struct ree_token_t *tokens, const int tokenCount){
  struct ree_token_t currentToken, previousToken;

  previousToken.token_type = TOKEN_ILLEGAL;

  for (int i = 0; i < tokenCount; ++i){
    currentToken = tokens[i];

    // unary detection
    if (currentToken.token_type == TOKEN_PLUS || currentToken.token_type == TOKEN_MINUS){
      // if the first token of the expression is a plus or a minus, its unary
      // e.g. -3 + 6 ...
      if (i == 0){
        // its a plus
        if (currentToken.token_type == TOKEN_PLUS){
          tokens[i].token_type = TOKEN_UNARY_PLUS;
        }

        // its a minus
        if (currentToken.token_type == TOKEN_MINUS){
          tokens[i].token_type = TOKEN_UNARY_MINUS;
        }
      }
      // e.g. (-3 + 6)
      else if (previousToken.token_type == TOKEN_PAREN_OPEN){
        // its a plus
        if (currentToken.token_type == TOKEN_PLUS){
          tokens[i].token_type = TOKEN_UNARY_PLUS;
        }

        // its a minus
        if (currentToken.token_type == TOKEN_MINUS){
          tokens[i].token_type = TOKEN_UNARY_MINUS;
        } 
      }
      else if (previousToken.token_type == TOKEN_FACTORIAL   || previousToken.token_type == TOKEN_DIVIDE
          ||   previousToken.token_type == TOKEN_MULTIPLY    || previousToken.token_type == TOKEN_POWER
          ||   previousToken.token_type == TOKEN_PLUS        || previousToken.token_type == TOKEN_MINUS
          ||   previousToken.token_type == TOKEN_UNARY_MINUS || previousToken.token_type == TOKEN_UNARY_PLUS
      ){
        // its a plus
        if (currentToken.token_type == TOKEN_PLUS){
          tokens[i].token_type = TOKEN_UNARY_PLUS;
        }

        // its a minus
        if (currentToken.token_type == TOKEN_MINUS){
          tokens[i].token_type = TOKEN_UNARY_MINUS;
        }
      }
    }

    previousToken = currentToken;
  }

  return ERR_SUCCESS;
}

enum reh_error_code_e ree_parseToPostfix(struct ree_token_t *tokens, const int tokenCount, struct ree_output_token_t *outputQueue, int *outputCount){
  *outputCount = 0;
  int operatorStackPointer = 0;
  struct ree_token_t stack[tokenCount];

  for (int i = 0; i < tokenCount; i++){
    struct ree_token_t currentToken = tokens[i];

    if (currentToken.token_type == TOKEN_NUMBER){
      struct ree_output_token_t outputToken = {
        .type = OUTPUT_NUMBER,
        .arity = 0,
        .value = strtof(currentToken.value, NULL)
      };
      strcpy(outputToken.symbol, currentToken.value);

      outputQueue[*outputCount] = outputToken;
      (*outputCount)++;
    }
    else if (currentToken.token_type == TOKEN_IDENTIFIER){ 
      // check if the identifier matches any of the supported functions 
      if (rgu_isStrInArray(validFunctions, functionArrLength, currentToken.value) == true){
        stack[operatorStackPointer++] = currentToken;
      }
      // check if its x
      else if (strcmp(currentToken.value, "x") == 0){
        struct ree_output_token_t outputToken = {
          .type = OUTPUT_NUMBER,
          .arity = 0,
          .value = 0.0f // the evaluator will substitute this
        };
        strcpy(outputToken.symbol, currentToken.value);

        outputQueue[*outputCount] = outputToken;
        (*outputCount)++;
      }
      else { // unknown identifier
        SET_ERROR_RETURN(ERR_UNKNOWN_IDENTIFIER, "Unknown identifier: %s", currentToken.value);
      }
    }
    else if (currentToken.token_type == TOKEN_PAREN_OPEN){
        stack[operatorStackPointer++] = currentToken;
    }
    else if (currentToken.token_type == TOKEN_PAREN_CLOSE){
      // pop operators until we find the matching '('

      bool foundMatchingParenthesis = false;

      // while operator stack isn't empty
      while (operatorStackPointer != 0){
        // pop from the stack (decrement the stackPointer before getting the structure -> we always get the structure one below the current stackPointer)
        struct ree_token_t topOperator = stack[--operatorStackPointer];

        // check if its the matching parenthesis
        if (topOperator.token_type == TOKEN_PAREN_OPEN){
          foundMatchingParenthesis = true;
          break;
        }
        else {
          struct ree_output_token_t outputToken = {
            .type = OUTPUT_OPERATOR,
            .arity = ree_determineArity(topOperator.token_type),
            .value = 0.0f // not used in the case of an operator
          };
          strcpy(outputToken.symbol, topOperator.value);

          outputQueue[*outputCount] = outputToken;
          (*outputCount)++;
        }
      }

      // check if we found a matching '('
      if (foundMatchingParenthesis == false){
        SET_ERROR_RETURN(ERR_MISMATCHED_PARENTHESES, "Mismatched parentheses: extra ')'");
      }

      // check if theres a function on top of the stack (e.g. "sin(...")
      if (operatorStackPointer != 0 && stack[operatorStackPointer - 1].token_type == TOKEN_IDENTIFIER){
        struct ree_token_t functionToken = stack[--operatorStackPointer];

        struct ree_output_token_t outputToken = {
          .type = OUTPUT_FUNCTION,
          .arity = ree_determineArity(functionToken.token_type),
          .value = 0.0f // not used in the case of an operator
        };
        strcpy(outputToken.symbol, functionToken.value);

        outputQueue[*outputCount] = outputToken;
        (*outputCount)++;
      }
    }
    else if (currentToken.token_type == TOKEN_FACTORIAL){
      struct ree_output_token_t outputToken = {
        .type = OUTPUT_FUNCTION,
        .arity = ree_determineArity(currentToken.token_type),
        .value = 0.0f // not used in the case of an operator
      };
      strcpy(outputToken.symbol, currentToken.value);

      outputQueue[*outputCount] = outputToken;
      (*outputCount)++;
    }
    else if ( // currentToken is ANY operator type
      currentToken.token_type == TOKEN_PLUS        ||
      currentToken.token_type == TOKEN_MINUS       ||
      currentToken.token_type == TOKEN_DIVIDE      ||
      currentToken.token_type == TOKEN_MULTIPLY    ||
      currentToken.token_type == TOKEN_POWER       ||
      currentToken.token_type == TOKEN_UNARY_PLUS  ||
      currentToken.token_type == TOKEN_UNARY_MINUS
    ){
      // get precedence and associativity of current operator
      int currentPrecedence;
      ree_operatorPrecedence(currentToken.token_type, &currentPrecedence);
      // ^ and unary -, + are the only right associativity operators
      int currentOpIsRightAssoc = (currentToken.token_type == TOKEN_POWER || 
                                    currentToken.token_type == TOKEN_UNARY_PLUS ||
                                    currentToken.token_type == TOKEN_UNARY_MINUS);

      // pop operators from stack with higher or equal precedence
      while (operatorStackPointer != 0){
        // peek at the top of the stack
        struct ree_token_t topOperator = stack[operatorStackPointer - 1];

        // stop if we hit a left parenthesis
        if (topOperator.token_type == TOKEN_PAREN_OPEN) break;

        int topPrecedence = 0;
        ree_operatorPrecedence(topOperator.token_type, &topPrecedence);

        /*
          Shuting Yard rule:
            pop if:
              topOperator has higher precedence
                            OR
              topOperator has equal precedence AND currentOperator is leftAssociative
        */
        bool shouldPop = false;

        if (topPrecedence > currentPrecedence) shouldPop = true;
        else if (topPrecedence == currentPrecedence && !currentOpIsRightAssoc) shouldPop = true;

        if (shouldPop == true){
          struct ree_token_t poppedOperator = stack[--operatorStackPointer];
 
          const char* symbolName;
          if (poppedOperator.token_type == TOKEN_UNARY_MINUS){
            symbolName = "NEG";
          }
          else if (poppedOperator.token_type == TOKEN_UNARY_PLUS){
            symbolName = "POS";
          }
          else {
            symbolName = poppedOperator.value;
          }

          struct ree_output_token_t outputToken = {
            .type = OUTPUT_OPERATOR,
            .arity = ree_determineArity(poppedOperator.token_type),
            .value = 0.0f // not used in the case of an operator
          };
          strcpy(outputToken.symbol, symbolName);

          outputQueue[*outputCount] = outputToken;
          (*outputCount)++;
        }
        else {
          break;
        }
      }

      // push onto the stack 
      stack[operatorStackPointer++] = currentToken;
    }
    else { // unknown token
      SET_ERROR_RETURN(ERR_INPUT_TOKEN_INVALID, "Unknown token type: %d", currentToken.token_type);
    }
  }

  // after the loop, pop remaining operators
  while (operatorStackPointer != 0){
    struct ree_token_t remainingOperator = stack[--operatorStackPointer];

    // if there is still a '(' on the stack, it's unmatched
    if (remainingOperator.token_type == TOKEN_PAREN_OPEN){
      SET_ERROR_RETURN(ERR_INVALID_INPUT, "Mismatched parentheses: extra '('");
    }

    enum ree_output_type_e outputTokenType = (remainingOperator.token_type == TOKEN_IDENTIFIER) ? OUTPUT_FUNCTION : OUTPUT_OPERATOR;

    struct ree_output_token_t outputToken = {
      .type =  outputTokenType,
      .arity = ree_determineArity(remainingOperator.token_type),
      .value = 0.0f // not used in the case of an operator
    };
    strcpy(outputToken.symbol, remainingOperator.value);

    outputQueue[*outputCount] = outputToken;
    (*outputCount)++;
  }

  return ERR_SUCCESS;
}
