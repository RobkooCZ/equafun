#include "utils/utilities.h"

#include <errno.h>
#include <string.h>

#ifdef WIN32
  #include <windows.h>
#else
  #include <time.h>
#endif

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

int rgu_msleep(long ms){
  #ifdef WIN32
    Sleep(ms);
  #else
    struct timespec ts;
    int result;

    if (ms < 0){
      errno = EINVAL; // invalid value
      return -1;
    }

    ts.tv_sec = ms / 1000;
    ts.tv_nsec = (ms % 1000) * 1000000;

    // sleep only when nanosleep doesn't get interrupted
    do {
      result = nanosleep(&ts, &ts);
    } while (result && errno == EINTR);

    return 0;
  #endif
}
