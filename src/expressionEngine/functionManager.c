#include "expressionEngine/functionManager.h"
#include "core/errorHandler.h"
#include <string.h>

enum reh_error_code_e ree_initFunctionManager(struct ree_function_manager_t *manager){
  if (manager == nullptr){
    SET_ERROR_RETURN(ERR_INVALID_POINTER, "Pointer to manager passed to ree_initFunctionManager is NULL.");
  }

  manager->functionCount = 0;
  memset(manager->functions, 0, sizeof(manager->functions));
  return ERR_SUCCESS;
}

int ree_getFunction(struct ree_function_manager_t *manager, const char *name, struct ree_function_t *function){
  for (int i = 0; i < manager->functionCount; ++i){
    if (strcmp(manager->functions[i].name, name) == 0){
      *function = manager->functions[i];
      return 0; // success
    }
  }

  return -1; // uh oh
}
