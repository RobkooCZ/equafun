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

// C11 type generic max macros

#define RETURN_2_NUMS { return a > b ? a : b; }

static int rm_MaxI(int a, int b) RETURN_2_NUMS
static long rm_MaxL(long a, long b) RETURN_2_NUMS
static unsigned int rm_MaxUI(unsigned int a, unsigned int b) RETURN_2_NUMS
static unsigned long rm_MaxUL(unsigned long a, unsigned long b) RETURN_2_NUMS
static float rm_MaxF(float a, float b) RETURN_2_NUMS
static double rm_MaxD(double a, double b) RETURN_2_NUMS
static long double rm_MaxLD(long double a, long double b) RETURN_2_NUMS

#define RM_MAX_2(a, b)                \
  _Generic((a) + (b),                 \
           long double: rm_MaxLD,     \
           double: rm_MaxD,           \
           float: rm_MaxF,            \
           long: rm_MaxL,             \
           int: rm_MaxI,              \
           unsigned long: rm_MaxUL,   \
           unsigned int: rm_MaxUI,    \
           default: rm_MaxD)          \
  ((a), (b))


#define RETURN_3_NUMS { return  (a > b ? (a > c ? a : c) : b); }

static int rm_Max3I(int a, int b, int c) RETURN_3_NUMS
static long rm_Max3L(long a, long b, long c) RETURN_3_NUMS
static unsigned int rm_Max3UI(unsigned int a, unsigned int b, unsigned int c) RETURN_3_NUMS
static unsigned long rm_Max3UL(unsigned long a, unsigned long b, unsigned long c) RETURN_3_NUMS
static float rm_Max3F(float a, float b, float c) RETURN_3_NUMS
static double rm_Max3D(double a, double b, double c) RETURN_3_NUMS
static long double rm_Max3LD(long double a, long double b, long double c) RETURN_3_NUMS

#define RM_MAX_3(a, b, c)             \
  _Generic((a) + (b) + (c),           \
           long double: rm_Max3LD,    \
           double: rm_Max3D,          \
           float: rm_Max3F,           \
           long: rm_Max3L,            \
           int: rm_Max3I,             \
           unsigned long: rm_Max3UL,  \
           unsigned int: rm_Max3UI,   \
           default: rm_Max3D)         \
  ((a), (b), (c))

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
