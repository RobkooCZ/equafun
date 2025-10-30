#ifndef FUNCTION_PARSER_H
#define FUNCTION_PARSER_H

#include "core/errorHandler.h"
#include "expressionEngine/functionManager.h"

enum reh_error_code_e ree_parseFunctionDefinition(char* definition, struct ree_function_t *function);

#endif // FUNCTION_PARSER_H
