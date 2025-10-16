#include "math/utility.h"
#include <float.h>
#include <math.h>

enum rm_num_classes_e rm_classifyNum(float num){
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
