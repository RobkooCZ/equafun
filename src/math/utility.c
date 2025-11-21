#include "math/utility.h"
#include "core/errorHandler.h"
#include "core/logger.h"
#include "renderer/graph.h"
#include <float.h>
#include <iso646.h>
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

double gammaLanczos(double x){
  // uses Lanczos approximation
  // source: https://en.wikipedia.org/wiki/Lanczos_approximation

  // Lanczos coefficients
  const int g = 7;
  static const double p[] = {
    0.99999999999980993,
    676.5203681218851,
    -1259.1392167224028,
    771.32342877765313,
    -176.61502916214059,
    12.507343278686905,
    -0.13857109526572012,
    9.9843695780195716e-6,
    1.5056327351493116e-7
  };

  if (x < 0.5){
    // Reflection formula
    return (double)PI / (sin((double)PI * x) * gammaLanczos(1.0 - x));
  }

  x -= 1.0;
  double a = p[0];
  for (size_t i = 1; i < sizeof(p) / sizeof(p[0]); ++i){
    a += p[i] / (x + (double)i);
  }

  double t = x + (double)g + 0.5;
  double res = sqrt(2.0 * (double)PI) * pow(t, x + 0.5) * exp(-t) * a;
  return res;
}

enum reh_error_code_e rm_Factorial(float z, float *result){
  if (result == nullptr){
    SET_ERROR_RETURN(ERR_INVALID_POINTER, "Pointer to result in rm_Factorial is NULL.");
  }

  double gammaVal = gammaLanczos((double)(z + 1.0f));
  if (gammaVal > (double)FLT_MAX){
    rl_LogMsg(RL_WARNING, "Result of factorial exceeded FLT_MAX.");
  }

  *result = (float)gammaVal;

  return ERR_SUCCESS;
}
