/**
  reh - Robkoo's Expression Engine
*/

#ifndef FUNCTION_PARSER_H
#define FUNCTION_PARSER_H

#include "core/errorHandler.h"
#include "expressionEngine/functionManager.h"

/**
  @brief Inserts multiplication token where implicit multiplication is detected
*/
enum reh_error_code_e ree_ImplicitMultiplication(struct ree_token_t **tokens, int *tokenCount, int *tokenCapacity);

/**
  @brief Parses a function definition string into a function structure
*/
enum reh_error_code_e ree_ParseFunction(char *definition, struct ree_function_t *function, struct ree_function_manager_t *manager, struct rm_vec3_t *functionColor);

#endif // FUNCTION_PARSER_H
