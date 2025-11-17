/**
  rm - Robkoo's Math
*/

#ifndef TYPE_CONVERT_H
#define TYPE_CONVERT_H

#include "math/Mat4.h"

/**
  @brief Gets a pointer to the underlying float array of the matrix
*/
enum reh_error_code_e rm_Mat4ValuePtr(struct rm_mat4_t *mat, float **result);

#endif 
