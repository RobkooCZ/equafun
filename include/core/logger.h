#ifndef LOGGER_H
#define LOGGER_H

#include "core/errorHandler.h"

// ASCII codes for colors for the different levels
#define DEBUG_CLR     "\x1B[1;36m"
#define SUCCESS_CLR   "\x1B[1;32m"
#define WARNING_CLR   "\x1B[1;33m"
#define ERROR_CLR     "\x1B[1;31m"
#define FAILURE_CLR   "\x1B[1;37m\x1B[41m"
#define END           "\x1B[0m"

// debug mode or no
#define _DEBUG_ENABLE

enum logLevel {
  DEBUG = -1, // every little detail
  SUCCESS = 0,// something went right
  WARNING = 1,// something isnt fully correct, but its more of a suggestion
  ERROR = 2,  // something went wrong but program continues running
  FAILURE = 3 // program cant continue functioning
};

void logMsg(enum logLevel severity, const char* __restrict__ msg, ...);

void logError(const ErrorContext *ctx, enum logLevel severity);
void logLastError(enum logLevel severity);
#endif // LOGGER_H
