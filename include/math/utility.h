/**
  rm - Robkoo's Math
*/

#ifndef MATH_UTILITIES_H
#define MATH_UTILITIES_H

#include "core/errorHandler.h"

typedef enum rm_num_classes_e {
  FUNC_ERROR   = -1,
  IS_NAN       =  0,
  IS_POSINF    =  1,
  IS_NEGINF    =  2,
  IS_ZERO      =  3,
  IS_NEGATIVE  =  4,
  IS_SUBNORMAL =  5,
  IS_NORMAL    =  6,
} NumClasses;

struct rm_complex_t {
  float real;
  float imag;
};

/**
  @brief Classifies a floating-point number into various categories
*/
enum rm_num_classes_e rm_ClassifyNum(float num);

/**
  @brief Calculates the aproximate value of the gamma function at x using the Lanczos aproximation
*/
double gammaLanczos(double x);
/**
  @brief Calculates the factorial of an integer
*/
enum reh_error_code_e rm_Factorial(float z, float *result);

#endif // MATH_UTILITES_H
