#include "utils/utilities.h"

#include <string.h>

bool rgu_isStrInArray(const char **arr, int arrSize, const char *str){
  for (int i = 0; i < arrSize; ++i){
    if (strcmp(arr[i], str) == 0){
      return true;
    }
  }

  return false;
}
