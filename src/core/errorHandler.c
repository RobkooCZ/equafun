#include "core/errorHandler.h"

#include <string.h>

static struct reh_error_context_t g_lastError = {0};

const struct reh_error_context_t *reh_GetLastError(void){
  return &g_lastError;
}

void reh_ClearError(void){
  memset(&g_lastError, 0, sizeof(struct reh_error_context_t));
}

void reh_SetError(enum reh_error_code_e code, const char* file, int line, const char* fnName, const char* message, const char* technicalInfo){
  if (file == nullptr) file = "Unknown File";
  if (fnName == nullptr) fnName = "Unknown Function";

  g_lastError.code = code;
  g_lastError.file = file;
  g_lastError.line = line;
  g_lastError.fnName = fnName;

  // prevent null-pointer bug
  if (message){
    strncpy(g_lastError.message, message, sizeof(g_lastError.message) - 1);
    g_lastError.message[sizeof(g_lastError.message) - 1] = '\0';
  }
  else {
    g_lastError.message[0] = '\0';
  }

  // technical info provided
  if (technicalInfo){
    strncpy(g_lastError.technicalDetails, technicalInfo, sizeof(g_lastError.technicalDetails) -1);
    g_lastError.technicalDetails[sizeof(g_lastError.technicalDetails) - 1] = '\0';
  }
  else { // technical info not provided
    g_lastError.technicalDetails[0] = '\0';
  }
}
