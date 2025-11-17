#include "utils/utilities.h"

#include <string.h>

bool rgu_IsStrInArray(const char **arr, int arrSize, const char *str){
  if (arr == nullptr || str == nullptr || arrSize <= 0){
    return false;
  }

  for (int i = 0; i < arrSize; ++i){
    if (strcmp(arr[i], str) == 0){
      return true;
    }
  }

  return false;
}

void rgu_TrimStr(char *str){
  if (str == nullptr){
    return;
  }

  int i = 0, j = 0;

  // count how many whitespaces there are
  while (str[i] == ' ') i++;

  // for example, if our str had 3 whitespaces, i would equal 2, j = 0. we would set the str[0] to the first non-whitespace value using i (due to i having count of the whitespaces) and repeat that until we reach the end of the string
  // when it copies the null terminator, the result of the assignement becomes 0, so the while loop breaks and we have our leading whitespaces trimmed!
  while ((str[j++] = str[i++]));
}
