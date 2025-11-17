#include "core/logger.h"
#include "core/errorHandler.h"

#include <stdio.h>
#include <stdarg.h>

#ifdef _WIN32
#define NOGDI // prevent inclusion of many stuff, amongst them being the RL_ERROR macro
#include <windows.h>

void rl_enableANSI(void){
  HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
  DWORD dwMode = 0;

  // get the current console mode
  if (!GetConsoleMode(hOut, &dwMode)){
    rl_LogMsg(RL_ERROR, "Failed to get console mode!");
  }

  // enable virtual terminal processing to support ANSI escape codes (works in PowerShell and CMD)
  dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;

  if (!SetConsoleMode(hOut, dwMode)){
    rl_LogMsg(RL_ERROR, "Failed to set console mode!");
  }
}

#endif

void rl_LogMsg(enum rl_log_level_e severity, const char *restrict msg, ...){
  #ifndef _DEBUG_ENABLE
    if (severity == RL_DEBUG) return; // do not print any debug message if the macro isnt set
  #endif

  char* color;
  char* tag;

  // assign color and tag based on severity
  switch (severity){
    case RL_DEBUG:   color = RL_DEBUG_COLOR;   tag = "DEBUG";   break;
    case RL_SUCCESS: color = RL_SUCCESS_COLOR; tag = "SUCCESS"; break;
    case RL_WARNING: color = RL_WARNING_COLOR; tag = "WARNING"; break;
    case RL_ERROR:   color = RL_ERROR_COLOR;   tag = "ERROR";   break;
    case RL_FAILURE: color = RL_FAILURE_COLOR; tag = "FAILURE"; break;
    default:      color = RL_END;         tag = "LOG";     break;
  }

  // print the bracket with colored tag
  printf("[%s %-7s %s] ", color, tag, RL_END);

  // print the message
  va_list args;
  va_start(args, msg);
  vprintf(msg, args);
  va_end(args);

  printf("\n");
}

void rl_LogError(const struct reh_error_context_t *ctx, enum rl_log_level_e severity){
  if (!ctx || ctx->code == ERR_SUCCESS) return; // dont log if we dont have context or if there is no error

  // always shown
  rl_LogMsg(severity, "%s", ctx->message);

  // Technical details and location (RL_DEBUG only (check handled in `rl_LogMsg()`))
  rl_LogMsg(RL_DEBUG, "\tLocation: %s:%d in %s()", ctx->file, ctx->line, ctx->fnName);
  if (ctx->technicalDetails[0] != '\0'){ // check if theres anything in the technicalDetails buffer
    rl_LogMsg(RL_DEBUG, "\tTechnical: %s", ctx->technicalDetails);
  }
}

void rl_LogLastError(enum rl_log_level_e severity){
  rl_LogError(reh_GetLastError(), severity);
}
