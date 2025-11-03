#include "core/errorHandler.h"

#include <string.h>

static ErrorContext g_lastError = {0};

const ErrorContext *getLastError(void){
  return &g_lastError;
}

void clearError(void){
  memset(&g_lastError, 0, sizeof(ErrorContext));
}

void setError(enum reh_error_code_e code, const char* file, int line, const char* fnName, const char* message, const char* technicalInfo){
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
