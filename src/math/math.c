// header files this file implements
#include "core/logger.h"
#include "math/Mat4.h"
#include "math/Vec2.h"

// std libs
#include <math.h>
#include <float.h>
#include <string.h>

// custom libs
#include "core/errorHandler.h"
#include "math/utility.h"

struct rm_vec2_val_res_t rm_ValidateVec2(struct rm_vec2_t v){
  struct rm_vec2_val_res_t result = {IS_NORMAL};

  result.xRes = rm_ClassifyNum(v.x);
  result.yRes = rm_ClassifyNum(v.y);

  return result;
}

enum reh_error_code_e rm_ReturnErrForNumClass(enum rm_num_classes_e class){
  switch (class){
    case IS_POSINF:
      return ERR_OVERFLOW;
    case IS_NEGINF:
      return ERR_UNDERFLOW;
    case IS_NAN:
      return ERR_VALUE_IS_NAN;
    default:
      return ERR_SUCCESS;
  }
}

/*
  VEC2
*/

enum reh_error_code_e rm_Vec2Add(struct rm_vec2_t a, struct rm_vec2_t b, struct rm_vec2_t *result){
  if (result == nullptr){
    SET_ERROR_RETURN(ERR_INVALID_POINTER, "Result pointer in rm_Vec2Add() is NULL");
  }

  VALIDATE_VEC2_INPUTS(a, b);

  result->x = a.x + b.x;
  result->y = a.y + b.y;

  return ERR_SUCCESS;
}

enum reh_error_code_e rm_Vec2Subtract(struct rm_vec2_t a, struct rm_vec2_t b, struct rm_vec2_t *result){
  if (result == nullptr){
    SET_ERROR_RETURN(ERR_INVALID_POINTER, "Result pointer in rm_Vec2Substract() is NULL");
  }

  VALIDATE_VEC2_INPUTS(a, b);
 
  result->x = a.x - b.x;
  result->y = a.y - b.y;

  return ERR_SUCCESS;
}

enum reh_error_code_e rm_Vec2Magnitude(struct rm_vec2_t v, float *result){
  if (result == nullptr){
    SET_ERROR_RETURN(ERR_INVALID_POINTER, "Result pointer in rm_Vec2Magnitude() is NULL");
  }

  VALIDATE_VEC2_INPUT(v);

  // ||v|| = sqrt(x^2 + y^2) for v = <x,y>
  *result = sqrtf(v.x * v.x + v.y * v.y);

  return ERR_SUCCESS;
}

enum reh_error_code_e rm_Vec2Normalize(struct rm_vec2_t v, struct rm_vec2_t *n_v){
  if (n_v == nullptr){
    SET_ERROR_RETURN(ERR_INVALID_POINTER, "Result pointer in rm_Vec2Normalize() is NULL");
  }

  VALIDATE_VEC2_INPUT(v);

  float magnitude = 0;
  CHECK_ERROR_CTX(rm_Vec2Magnitude(v, &magnitude), "Failed to get vector magnitude.");

  if (magnitude < FLT_EPSILON){
    rl_LogMsg(RL_WARNING, "Magnitude too close to 0 (%.2e), setting normalized vector to: <0,0>", (double)magnitude);
    n_v->x = 0;
    n_v->y = 0;
    return ERR_SUCCESS;
  }

  /*        v             v
    n_v = ----- = ----------------- | for v = <x,y>
          ||v||    sqrt(x^2 + y^2)

    note: if ||v|| comes out to 0, a normalized vector of <0,0> is returned to prevent division by 0.
  */

  n_v->x = v.x / magnitude;
  n_v->y = v.y / magnitude;

  return ERR_SUCCESS;
}

enum reh_error_code_e rm_Vec2Scale(struct rm_vec2_t v, float scalar, struct rm_vec2_t *result){
  if (result == nullptr){
    SET_ERROR_RETURN(ERR_INVALID_POINTER, "Result pointer in rm_Vec2Scale() is NULL");
  }

  VALIDATE_VEC2_INPUT(v);

  // validate scalar
  enum rm_num_classes_e scalarClass = rm_ClassifyNum(scalar);
  enum reh_error_code_e scalarErr = rm_ReturnErrForNumClass(scalarClass);
  if (scalarErr != ERR_SUCCESS){
    SET_ERROR_RETURN(scalarErr, "Invalid scalar provided to rm_Vec2Scale()");
  }

  /*
    v * scalar = <v.x * scalar, v.y * scalar> for v = <x,y>
  */

  result->x = v.x * scalar;
  result->y = v.y * scalar;

  return ERR_SUCCESS;
}

enum reh_error_code_e rm_Vec2Dot(struct rm_vec2_t a, struct rm_vec2_t b, float *result){
  if (result == nullptr){
    SET_ERROR_RETURN(ERR_INVALID_POINTER, "Result pointer in rm_Vec2Dot() is NULL");
  }

  VALIDATE_VEC2_INPUTS(a, b);

  *result = a.x * b.x + a.y * b.y;

  return ERR_SUCCESS;
}

enum reh_error_code_e rm_Vec2Distance(struct rm_vec2_t a, struct rm_vec2_t b, float *result){
  if (result == nullptr){
    SET_ERROR_RETURN(ERR_INVALID_POINTER, "Result pointer in rm_Vec2Distance() is NULL");
  }

  VALIDATE_VEC2_INPUTS(a, b);

  // d = sqrt((x_2 - x_1)^2 + (y_2 - y_1)^2) where x, y are points (or 2D vectors)
  // i use my previous functions here for composability
  struct rm_vec2_t difference = {0};
  CHECK_ERROR_CTX(rm_Vec2Subtract(a, b, &difference), "Failed to compute vector difference.");
  CHECK_ERROR_CTX(rm_Vec2Magnitude(difference, result), "Failed to compute vector magnitude.");

  return ERR_SUCCESS;
}

/*
  VEC3 (no funcs yet)
*/

/*
  MAT4
*/

enum reh_error_code_e rm_Mat4Get(struct rm_mat4_t *mat, int row, int col, float *result){
  if (mat == nullptr) {
    SET_ERROR_RETURN(ERR_INVALID_POINTER, "Matrix pointer in rm_Mat4Get() is NULL");
  }

  if (result == nullptr) {
    SET_ERROR_RETURN(ERR_INVALID_POINTER, "Result pointer in rm_Mat4Get() is NULL");
  }

  // Bounds checking
  if (row < 0 || row >= 4) {
    SET_ERROR_RETURN(ERR_OUT_OF_BOUNDS, "Row index %d out of bounds [0, 3] in rm_Mat4Get()", row);
  }

  if (col < 0 || col >= 4) {
    SET_ERROR_RETURN(ERR_OUT_OF_BOUNDS, "Column index %d out of bounds [0, 3] in rm_Mat4Get()", col);
  }

  // Column-major order: data[col * 4 + row]
  *result = mat->data[col * 4 + row];

  return ERR_SUCCESS;
}

enum reh_error_code_e rm_Mat4Ortho(float left, float right, float bottom, float top, struct rm_mat4_t *result){
  if (result == nullptr) {
    SET_ERROR_RETURN(ERR_INVALID_POINTER, "Result pointer in rm_Mat4Ortho() is NULL");
  }

  // Validate inputs (prevent division by zero)
  if (fabsf(right - left) < FLT_EPSILON) {
    SET_ERROR_RETURN(ERR_INVALID_INPUT, "right == left in rm_Mat4Ortho() (division by zero)");
  }
  if (fabsf(top - bottom) < FLT_EPSILON){
    SET_ERROR_RETURN(ERR_INVALID_INPUT, "top == bottom in rm_Mat4Ortho() (division by zero)");
  }

  // Initialize matrix to zero
  memset(result->data, 0, sizeof(result->data));

  // Orthographic projection formula (column-major order)
  result->data[0]  = 2.0f / (right - left);              // [0][0]
  result->data[5]  = 2.0f / (top - bottom);              // [1][1]
  result->data[10] = -1.0f;                              // [2][2]
  result->data[12] = -(right + left) / (right - left);   // [3][0]
  result->data[13] = -(top + bottom) / (top - bottom);   // [3][1]
  result->data[15] = 1.0f;                               // [3][3]

  return ERR_SUCCESS;
}

