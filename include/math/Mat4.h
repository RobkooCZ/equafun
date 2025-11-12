#ifndef MAT4_H
#define MAT4_H

#include "core/errorHandler.h"

// defined as a 1D array for OpenGL compatibilty
typedef struct rm_mat4_t {
  float data[16];
} Mat4;

enum reh_error_code_e rm_Mat4Get(struct rm_mat4_t *mat, int row, int col, float *result);
enum reh_error_code_e rm_Mat4Ortho(float left, float right, float bottom, float top, struct rm_mat4_t *result);

#endif // MAT4_H
