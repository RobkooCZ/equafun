#ifndef MATH_UTILITIES_H
#define MATH_UTILITIES_H

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

// Function that returns one of the above enum values based on the number provided.
// Used in math functions for edge case checking
enum rm_num_classes_e rm_classifyNum(float num);

#endif // MATH_UTILITES_H
