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

#endif // UTILITIES_H
