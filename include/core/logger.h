/**
  rl - Robkoo's Logger
*/

#ifndef LOGGER_H
#define LOGGER_H

#include "core/errorHandler.h"

#ifdef __unix__
  #include <bits/types/siginfo_t.h>
#endif

// ASCII codes for colors for the different levels
#ifdef __linux__
  #define RL_DEBUG_COLOR     "\x1B[1;36m"
  #define RL_SUCCESS_COLOR   "\x1B[1;32m"
  #define RL_WARNING_COLOR   "\x1B[1;33m"
  #define RL_ERROR_COLOR     "\x1B[1;31m"
  #define RL_FAILURE_COLOR   "\x1B[1;37m\x1B[41m"
  #define RL_END           "\x1B[0m"

  // SIGSEGV printing/handling functions for Linux. Windows is a tad more complicated, so i couldnt care less rn.

  /**
    @brief Prints the backtrace using rl_LogMsg.
  */
  void rl_PrintBacktrace(void);

  /**
    @brief Handles segfault by printing all the available info and then dumping the core.
  */
  void rl_HandleSegfault(int signum, siginfo_t *info, void *context);

#endif

#ifdef _WIN32
  #define RL_DEBUG_COLOR     "\x1B[36m"
  #define RL_SUCCESS_COLOR   "\x1B[32m"
  #define RL_WARNING_COLOR   "\x1B[33m"
  #define RL_ERROR_COLOR     "\x1B[31m"
  #define RL_FAILURE_COLOR   "\x1B[37m\x1B[41m"
  #define RL_END           "\x1B[0m"

  // on Windows you have to enable virtual terminal processing so it can handle ANSI escape codes (for colour)
  void rl_enableANSI(void);
#endif

#define _DEBUG_ENABLE
#define RL_BACKTRACE_COUNT 10

enum rl_log_level_e {
  RL_DEBUG = -1, // every little detail
  RL_SUCCESS = 0,// something went right
  RL_WARNING = 1,// something isnt fully correct, but its more of a suggestion
  RL_ERROR = 2,  // something went wrong but program continues running
  RL_FAILURE = 3 // program cant continue functioning
};

/**
  @brief Logs a message to the console with a given severity.
*/
void rl_LogMsg(enum rl_log_level_e severity, const char* __restrict__ msg, ...);

/**
  @brief Logs an error context to the console with a given severity.
*/
void rl_LogError(const struct reh_error_context_t *ctx, enum rl_log_level_e severity);

/**
  @brief Logs the last error context to the console with a given severity.
*/
void rl_LogLastError(enum rl_log_level_e severity);


#endif // LOGGER_H
