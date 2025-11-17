/**
  rm - Robkoo's Math
*/

#ifndef MAT4_H
#define MAT4_H

#include "core/errorHandler.h"

// defined as a 1D array for OpenGL compatibilty
typedef struct rm_mat4_t {
  float data[16]; /**< 4x4 matrix data in column-major order */
} Mat4;

/**
  @brief Sets a value in the matrix at the specified row and column
*/
enum reh_error_code_e rm_Mat4Get(struct rm_mat4_t *mat, int row, int col, float *result);

/**
  @brief Creates an orthographic projection matrix
*/
enum reh_error_code_e rm_Mat4Ortho(float left, float right, float bottom, float top, struct rm_mat4_t *result);

#endif // MAT4_H
