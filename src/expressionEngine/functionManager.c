#include "expressionEngine/functionManager.h"
#include "core/errorHandler.h"
#include "expressionEngine/parser/functionParser.h"
#include <stdlib.h>
#include <string.h>
#include "core/logger.h"

struct rm_vec3_t functionColorArray[] = {RED, ORANGE, YELLOW, GREEN, BLUE, PURPLE, PINK, GRAY, WHITE};
const int functionColorArrayLength = sizeof(functionColorArray) / sizeof(functionColorArray[0]);

enum reh_error_code_e ree_InitFunctionManager(struct ree_function_manager_t *manager){
  if (manager == nullptr){
    SET_ERROR_RETURN(ERR_INVALID_POINTER, "Pointer to manager passed to ree_InitFunctionManager is NULL.");
  }

  manager->functionCount = 0;
  memset(manager->functions, 0, sizeof(manager->functions));
  return ERR_SUCCESS;
}

int ree_GetFunction(struct ree_function_manager_t *manager, const char *name, struct ree_function_t *function){
  if (manager == nullptr){
    return -1;
  }
  else if (name == nullptr){
    return -1;
  }
  else if (function == nullptr){
    return -1;
  }

  for (int i = 0; i < manager->functionCount; ++i){
    if (strcmp(manager->functions[i].name, name) == 0){
      *function = manager->functions[i];
      return 0; // success
    }
  }

  return -1; // uh oh
}

bool ree_IsFunctionInManager(struct ree_function_manager_t *manager, const char* name){
  for (int i = 0; i < manager->functionCount; ++i){
    if (strcmp(manager->functions[i].name, name) == 0){
      return true; // yes it is
    }
  }
  return false; // no its not
}

enum reh_error_code_e ree_AddFunction(struct ree_function_manager_t *manager, char *definition, struct rm_vec3_t *functionColor){
  if (manager == nullptr){
    SET_ERROR_RETURN(ERR_INVALID_POINTER, "Manager passed to ree_AddFunction is NULL.");
  }
  else if (definition == nullptr){
    SET_ERROR_RETURN(ERR_INVALID_POINTER, "Definition passed to ree_AddFunction is NULL.");
  }
  else if (manager->functionCount >= REE_MAX_FUNCTIONS){
    SET_ERROR_RETURN(ERR_OUT_OF_MEMORY, "Function manager has reached maximum function capacity.");
  }
  else if (functionColor == nullptr){
    rl_LogMsg(RL_WARNING, "No function color (NULL) provided to ree_AddFunction, assigning next available color.");
    // assign a color from the array based on the current function count
    functionColor = &functionColorArray[manager->functionCount % functionColorArrayLength];
  }

  // add the function to the manager
  CHECK_ERROR_CTX(ree_ParseFunction(definition, &manager->functions[manager->functionCount], manager, functionColor), "Failed to parse function definition.");

  manager->functionCount++;

  return ERR_SUCCESS;
}

enum reh_error_code_e ree_RemoveFunction(struct ree_function_manager_t *manager, const char *name){
  if (manager == nullptr){
    SET_ERROR_RETURN(ERR_INVALID_POINTER, "Manager passed to ree_RemoveFunction is NULL.");
  }
  else if (name == nullptr){
    SET_ERROR_RETURN(ERR_INVALID_POINTER, "Name passed to ree_RemoveFunction is NULL.");
  }

  bool functionExists = false;
  int functionPos;

  // check that the name exists in the function manager
  for (functionPos = 0; functionPos < manager->functionCount; ++functionPos){
    // if the name of the function on the ith position is the same as the provided name
    if (strcmp(manager->functions[functionPos].name, name) == 0){
      functionExists = true;
      break;
    }
  }

  if (functionExists == false){
    SET_ERROR_RETURN(ERR_INVALID_INPUT, "Function with name %s passed to ree_RemoveFunction wasn't found.", name);
  }

  // free the function data
  free(manager->functions[functionPos].tokens);
  free(manager->functions[functionPos].rpn);

  // move the functions that were after this removed one back (to not have holes in the arr)
  memmove(&manager->functions[functionPos], &manager->functions[functionPos + 1], (long unsigned int)(manager->functionCount - functionPos - 1) * sizeof *manager->functions);

  manager->functionCount--;

  // clear the old data left in the last slot after moving the array
  memset(&manager->functions[manager->functionCount], 0, sizeof manager->functions[manager->functionCount]);

  return ERR_SUCCESS;
}
