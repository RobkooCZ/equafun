#include "core/logger.h"
#include "core/errorHandler.h"

#include <stdio.h>
#include <stdarg.h>

void logMsg(enum logLevel severity, const char *restrict msg, ...){
  #ifndef _DEBUG_ENABLE
    if (severity == DEBUG) return; // do not print any debug message if the macro isnt set
  #endif

  char* color;
  char* tag;

  // assign color and tag based on severity
  switch (severity){
    case DEBUG:   color = DEBUG_CLR;   tag = "DEBUG";   break;
    case SUCCESS: color = SUCCESS_CLR; tag = "SUCCESS"; break;
    case WARNING: color = WARNING_CLR; tag = "WARNING"; break;
    case ERROR:   color = ERROR_CLR;   tag = "ERROR";   break;
    case FAILURE: color = FAILURE_CLR; tag = "FAILURE"; break;
    default:      color = END;         tag = "LOG";     break;
  }

  // print the bracket with colored tag
  printf("[%s %-7s %s] ", color, tag, END);

  // print the message
  va_list args;
  va_start(args, msg);
  vprintf(msg, args);
  va_end(args);

  printf("\n");
}

void logError(const ErrorContext *ctx, enum logLevel severity){
  if (!ctx || ctx->code == ERR_SUCCESS) return; // dont log if we dont have context or if there is no error

  // always shown
  logMsg(severity, "%s", ctx->message);

  // Technical details and location (DEBUG only (check handled in `logMsg()`))
  logMsg(DEBUG, "\tLocation: %s:%d in %s()", ctx->file, ctx->line, ctx->fnName);
  if (ctx->technicalDetails[0] != '\0'){ // check if theres anything in the technicalDetails buffer
    logMsg(DEBUG, "\tTechnical: %s", ctx->technicalDetails);
  }
}

void logLastError(enum logLevel severity){
  logError(getLastError(), severity);
}
