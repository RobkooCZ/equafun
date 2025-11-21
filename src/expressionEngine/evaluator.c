#include "expressionEngine/evaluator.h"
#include "core/errorHandler.h"
#include "expressionEngine/parser/shuntingYard.h"
#include "math/utility.h"
#include "core/logger.h"

#include <ctype.h>
#include <float.h>
#include <math.h>
#include <string.h>

#define POP_2_NUMS()                    \
    if (stackIndex < 2){                \
      SET_ERROR_RETURN(ERR_INVALID_MEMORY_ACCESS, "Stack index whilst popping two numbers would've gone below 0. Stack index value: %zu", stackIndex);                                  \
    }                                   \
    float num2 = stack[--stackIndex];   \
    float num1 = stack[--stackIndex];

#define POP_1_NUM()                     \
    if (stackIndex < 1){                \
      SET_ERROR_RETURN(ERR_INVALID_MEMORY_ACCESS, "Stack index whilst popping a number would've gone below 0.Stack index value: %zu", stackIndex);                                     \
    }                                   \
    float num1 = stack[--stackIndex];

enum reh_error_code_e ree_EvaluateRpn(struct ree_output_token_t *rpn, size_t rpnCount, struct ree_variable_t *variables, size_t variableCount, float *result){
  if (rpn == nullptr){
    SET_ERROR_RETURN(ERR_INVALID_POINTER, "RPN provided to ree_EvaluateRpn is NULL.");
  }
  if (variables == nullptr && variableCount != 0){
    SET_ERROR_RETURN(ERR_INVALID_POINTER, "Variables provided to ree_EvaluateRpn are NULL.");
  }
  if (result == nullptr){
    SET_ERROR_RETURN(ERR_INVALID_POINTER, "Result provided to ree_EvaluateRpn is NULL.");
  }

  if (rpnCount == 0){
    SET_ERROR_RETURN(ERR_INVALID_INPUT, "rpnCount provided to ree_EvaluateRpn is 0.");
  }

  float stack[rpnCount];
  size_t stackIndex = 0;

  for (size_t i = 0; i < rpnCount; ++i){
    // if the token is a number, push it to the stack
    if (rpn[i].type == OUTPUT_NUMBER){
      // check if the number is a variable that we need to substitute for
      if (isalpha((unsigned char)rpn[i].symbol[0]) && variableCount != 0){
        bool variableFound = false;
        for (size_t j = 0; j < variableCount; ++j){
          if (strcmp(rpn[i].symbol, variables[j].name) == 0){
            stack[stackIndex++] = variables[j].value;
            variableFound = true;
            break;
          }
        }

        // variable not found, can't substitute
        if (variableFound == false) SET_ERROR_RETURN(ERR_INVALID_INPUT, "No value provided for variable %s in ree_EvaluateRpn.", rpn[i].symbol);
      }
      // error - symbol found but no variable (well, variableCount is just zero) was provided
      else if (isalpha((unsigned char)rpn[i].symbol[0]) && variableCount == 0){
       SET_ERROR_RETURN(ERR_INVALID_INPUT, "Found identifier (%s) in RPN expression, but no variable value was provided to substitute for it.", rpn[i].symbol);
      }
      // its a number
      else {
        stack[stackIndex++] = rpn[i].value;
      }
    }
    // if the token is an operator or a function, pop the required amount of numbers from the stack, process the calcualtion and put the result to the stack
    else if (rpn[i].type == OUTPUT_OPERATOR || rpn[i].type == OUTPUT_FUNCTION){
      /*
        #############
        # OPERATORS #
        #############
      */
      if (strcmp(rpn[i].symbol, "+") == 0){
        POP_2_NUMS();
        // perform operation and push it to the stack
        stack[stackIndex++] = num1 + num2;
      }
      else if (strcmp(rpn[i].symbol, "-") == 0){
        POP_2_NUMS();
        // perform operation and push it to the stack
        stack[stackIndex++] = num1 - num2;
      }
      else if (strcmp(rpn[i].symbol, "*") == 0){
        POP_2_NUMS();
        // perform operation and push it to the stack
        stack[stackIndex++] = num1 * num2;
      }
      else if (strcmp(rpn[i].symbol, "/") == 0){
        POP_2_NUMS();
        // check for division by zero
        if (fabsf(num2) < FLT_EPSILON){
          SET_ERROR_RETURN(ERR_DIVISION_BY_ZERO, "Attempted to divide by zero while evaluation expression.");
        }
        // perform operation and push it to the stack
        stack[stackIndex++] = num1 / num2;
      }
      else if (strcmp(rpn[i].symbol, "^") == 0){
        POP_2_NUMS();
        stack[stackIndex++] = powf(num1, num2);
      }
      else if (strcmp(rpn[i].symbol, "!") == 0){
        POP_1_NUM();

        float localResult;
        CHECK_ERROR_CTX(rm_Factorial(num1, &localResult), "Failed to calculate factorial.");
        stack[stackIndex++] = localResult;
      }
      else if (strcmp(rpn[i].symbol, "NEG") == 0){
        POP_1_NUM();
        stack[stackIndex++] = -num1;
      }
      else if (strcmp(rpn[i].symbol, "POS") == 0){
        POP_1_NUM();
        stack[stackIndex++] = +num1;
      }

      /*
        #############
        # FUNCTIONS #
        #############
      */
      else if (strcmp(rpn[i].symbol, "sin") == 0){
        POP_1_NUM();
        stack[stackIndex++] = sinf(num1);
      }
      else if (strcmp(rpn[i].symbol, "cos") == 0){
        POP_1_NUM();
        stack[stackIndex++] = cosf(num1);
      }
      else if (strcmp(rpn[i].symbol, "tan") == 0){
        POP_1_NUM();
        if (fabsf(cosf(num1)) < FLT_EPSILON){
          SET_ERROR_RETURN(ERR_TAN_OUT_OF_DOMAIN, "Tan is undefined for x = %f", (double)num1);
        }
        stack[stackIndex++] = tanf(num1);
      }
      else if (strcmp(rpn[i].symbol, "sqrt") == 0){
        POP_1_NUM();
        if (num1 < 0){
          SET_ERROR_RETURN(ERR_INVALID_SQRT, "Sqrt is undefined for x = %f", (double)num1);
        }
        stack[stackIndex++] = sqrtf(num1);
      }
      else if (strcmp(rpn[i].symbol, "abs") == 0){
        POP_1_NUM();
        stack[stackIndex++] = fabsf(num1);
      }
      else if (strcmp(rpn[i].symbol, "ln") == 0){
        POP_1_NUM();
        if (num1 <= 0){
          SET_ERROR_RETURN(ERR_LN_OUT_OF_DOMAIN, "Natural log of x is undefined for %f", (double)num1);
        }
        // should be lnf() ;)
        stack[stackIndex++] = logf(num1);
      }
      else if (strcmp(rpn[i].symbol, "log") == 0){
        POP_1_NUM();
        if (num1 <= 0){
          SET_ERROR_RETURN(ERR_LOG_OUT_OF_DOMAIN, "Log with base 10 of x is undefined for %f", (double)num1);
        }
        // should be logf() ;)
        stack[stackIndex++] = log10f(num1);
      }
      /*
        #####################
        # UNKNOWN RPN TOKEN #
        #####################
      */
      else {
        SET_ERROR_RETURN(ERR_INVALID_OPERATOR, "Provided RPN token (%s) is unknown.", rpn[i].symbol);
      }
    }
  }

  // result SHOULD be the only thing left on stack
  if (stackIndex != 1){
    SET_ERROR_RETURN(ERR_INVALID_STACK_STATE, "There is more than the result on the stack left. (stackIndex: %zu)", stackIndex);
  }
  *result = stack[0];

  return ERR_SUCCESS;
}
