/**
  rgu - Robkoo's General Utilities
*/

#ifndef UTILITIES_H
#define UTILITIES_H

/**
  @brief Checks if a string is present in an array of strings
*/
bool rgu_IsStrInArray(const char **arr, int arrSize, const char* str);

/**
  @brief Trims whitespace from a string
*/
void rgu_TrimStr(char *str);

/**
  @brief Suspends the caller thread for `ms` miliseconds
*/
int rgu_msleep(long ms);

/**
  @brief Hashes the input string with the djb3 algorithm and returns the hash as an int
*/
int rgu_hash(const char* input);

#endif // UTILITIES_H
