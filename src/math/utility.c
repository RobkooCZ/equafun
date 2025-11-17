#include "math/utility.h"
#include "core/errorHandler.h"
#include <float.h>
#include <math.h>

enum rm_num_classes_e rm_ClassifyNum(float num){
  switch (fpclassify(num)){
    case FP_NAN:
      return IS_NAN;
    case FP_INFINITE:
      if (num < 0.0f) return IS_NEGINF;
      else return IS_POSINF;
    case FP_ZERO:
      return IS_ZERO;
    case FP_SUBNORMAL:
      return IS_SUBNORMAL;
    case FP_NORMAL:
      if (num < 0) return IS_NEGATIVE;
      else return IS_NORMAL;
    default:
      return FUNC_ERROR;
  }
}

enum reh_error_code_e rm_Factorial(int n, int *result){
  if (result == nullptr){
    SET_ERROR_RETURN(ERR_INVALID_POINTER, "Pointer to result in rm_Factorial is NULL.");
  }

  if (n == 0){
    *result = 1;
    return ERR_SUCCESS;
  }
  else if (n < 0){
    *result = 0;
    SET_ERROR_RETURN(ERR_INVALID_INPUT, "n lower than 0 (%d) in rm_Factorial.", n);
  }

  *result = 1;

  for (int i = 1; i <= n; ++i){
    *result *= i;
  }

  return ERR_SUCCESS;
}
