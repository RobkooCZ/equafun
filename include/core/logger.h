#ifndef LOGGER_H
#define LOGGER_H

// ASCII codes for colors for the different levels
#define DEBUG_CLR     "\e[1;36m"
#define SUCCESS_CLR   "\e[1;32m"
#define WARNING_CLR   "\e[1;33m"
#define ERROR_CLR     "\e[1;31m"
#define FAILURE_CLR   "\e[1;37m\e[41m"
#define END           "\e[0m"

// debug mode or no
// #define _DEBUG_ENABLE

enum logLevel {
  DEBUG = -1, // every little detail
  SUCCESS = 0,// something went right
  WARNING = 1,// something isnt fully correct, but its more of a suggestion
  ERROR = 2,  // something went wrong but program continues running
  FAILURE = 3 // program cant continue functioning
};

void logMsg(enum logLevel severity, const char* __restrict__ msg, ...);

#endif // LOGGER_H
