/*
`rm_...` is the identifier for the set of Robkoos math functions.
internally, `struct rm_vec2_t` is used instead of Vec2.
*/

#ifndef VEC2_H
#define VEC2_H

#include "core/errorHandler.h"
#include "utility.h"

typedef struct rm_vec2_t {
  float x;
  float y;
} Vec2;

struct rm_vec2_val_res_t {
  enum rm_num_classes_e xRes;
  enum rm_num_classes_e yRes;
};

// helper macros to reduce boiler plate
// general check
#define VALIDATE_VEC2_INPUTS(a, b)                                    \
  do {                                                                \
    struct rm_vec2_val_res_t aRes = rm_ValidateVec2(a);               \
    struct rm_vec2_val_res_t bRes = rm_ValidateVec2(b);               \
                                                                      \
    enum reh_error_code_e aXErr = rm_ReturnErrForNumClass(aRes.xRes);        \
    if (aXErr != ERR_SUCCESS){                                        \
      SET_ERROR_RETURN(aXErr, "Invalid x component in first vector"); \
    }                                                                 \
                                                                      \
    enum reh_error_code_e aYErr = rm_ReturnErrForNumClass(aRes.yRes);        \
    if (aYErr != ERR_SUCCESS){                                        \
      SET_ERROR_RETURN(aYErr, "Invalid y component in first vector"); \
    }                                                                 \
                                                                      \
    enum reh_error_code_e bXErr = rm_ReturnErrForNumClass(bRes.xRes);        \
    if (bXErr != ERR_SUCCESS){                                        \
      SET_ERROR_RETURN(bXErr, "Invalid x component in second vector");\
    }                                                                 \
                                                                      \
    enum reh_error_code_e bYErr = rm_ReturnErrForNumClass(bRes.yRes);        \
    if (bYErr != ERR_SUCCESS){                                        \
      SET_ERROR_RETURN(bYErr, "Invalid y component in second vector");\
    }                                                                 \
  } while(0)

// general check
#define VALIDATE_VEC2_INPUT(a)                                        \
  do {                                                                \
    struct rm_vec2_val_res_t aRes = rm_ValidateVec2(a);               \
                                                                      \
    enum reh_error_code_e aXErr = rm_ReturnErrForNumClass(aRes.xRes);        \
    if (aXErr != ERR_SUCCESS){                                        \
      SET_ERROR_RETURN(aXErr, "Invalid x component in input vector"); \
    }                                                                 \
                                                                      \
    enum reh_error_code_e aYErr = rm_ReturnErrForNumClass(aRes.yRes);        \
    if (aYErr != ERR_SUCCESS){                                        \
      SET_ERROR_RETURN(aYErr, "Invalid y component in input vector"); \
    }                                                                 \
  } while(0)

struct rm_vec2_val_res_t rm_ValidateVec2(struct rm_vec2_t);

enum reh_error_code_e rm_ReturnErrForNumClass(enum rm_num_classes_e class);

enum reh_error_code_e rm_Vec2Add(struct rm_vec2_t a, struct rm_vec2_t b, struct rm_vec2_t *result);

enum reh_error_code_e rm_Vec2Subtract(struct rm_vec2_t a, struct rm_vec2_t b, struct rm_vec2_t *result);

enum reh_error_code_e rm_Vec2Magnitude(struct rm_vec2_t v, float *result);

enum reh_error_code_e rm_Vec2Normalize(struct rm_vec2_t v, struct rm_vec2_t *n_v);

enum reh_error_code_e rm_Vec2Scale(struct rm_vec2_t v, float scalar, struct rm_vec2_t *result);

enum reh_error_code_e rm_Vec2Dot(struct rm_vec2_t a, struct rm_vec2_t b, float *result);

enum reh_error_code_e rm_Vec2Distance(struct rm_vec2_t a, struct rm_vec2_t b, float *result);

#endif // VEC2_H
