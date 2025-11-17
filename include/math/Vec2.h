/*
  rm - Robkoo's Math
*/

#ifndef VEC2_H
#define VEC2_H

#include "core/errorHandler.h"
#include "utility.h"

typedef struct rm_vec2_t {
  float x; /**< x component of the vector */
  float y; /**< y component of the vector */
} Vec2;

struct rm_vec2_val_res_t {
  enum rm_num_classes_e xRes; /**< classification result for x component */
  enum rm_num_classes_e yRes; /**< classification result for y component */
};

// helper macros to reduce boiler plate
// general check
#define VALIDATE_VEC2_INPUTS(a, b)                                    \
  do {                                                                \
    struct rm_vec2_val_res_t aRes = rm_ValidateVec2(a);               \
    struct rm_vec2_val_res_t bRes = rm_ValidateVec2(b);               \
    enum reh_error_code_e aXErr = rm_ReturnErrForNumClass(aRes.xRes); \
    if (aXErr != ERR_SUCCESS){                                        \
      SET_ERROR_RETURN(aXErr, "Invalid x component in first vector"); \
    }                                                                 \
    enum reh_error_code_e aYErr = rm_ReturnErrForNumClass(aRes.yRes); \
    if (aYErr != ERR_SUCCESS){                                        \
      SET_ERROR_RETURN(aYErr, "Invalid y component in first vector"); \
    }                                                                 \
    enum reh_error_code_e bXErr = rm_ReturnErrForNumClass(bRes.xRes); \
    if (bXErr != ERR_SUCCESS){                                        \
      SET_ERROR_RETURN(bXErr, "Invalid x component in second vector");\
    }                                                                 \
    enum reh_error_code_e bYErr = rm_ReturnErrForNumClass(bRes.yRes); \
    if (bYErr != ERR_SUCCESS){                                        \
      SET_ERROR_RETURN(bYErr, "Invalid y component in second vector");\
    }                                                                 \
  } while(0)

// general check
#define VALIDATE_VEC2_INPUT(a)                                        \
  do {                                                                \
    struct rm_vec2_val_res_t aRes = rm_ValidateVec2(a);               \
    enum reh_error_code_e aXErr = rm_ReturnErrForNumClass(aRes.xRes); \
    if (aXErr != ERR_SUCCESS){                                        \
      SET_ERROR_RETURN(aXErr, "Invalid x component in input vector"); \
    }                                                                 \
    enum reh_error_code_e aYErr = rm_ReturnErrForNumClass(aRes.yRes); \
    if (aYErr != ERR_SUCCESS){                                        \
      SET_ERROR_RETURN(aYErr, "Invalid y component in input vector"); \
    }                                                                 \
  } while(0)

/**
  @brief Validates the components of a 2D vector
*/
struct rm_vec2_val_res_t rm_ValidateVec2(struct rm_vec2_t);

/**
  @brief Returns an error code based on the classification of a number
*/
enum reh_error_code_e rm_ReturnErrForNumClass(enum rm_num_classes_e class);

/**
  @brief Adds two 2D vectors
*/
enum reh_error_code_e rm_Vec2Add(struct rm_vec2_t a, struct rm_vec2_t b, struct rm_vec2_t *result);

/**
  @brief Subtracts the second 2D vector from the first
*/
enum reh_error_code_e rm_Vec2Subtract(struct rm_vec2_t a, struct rm_vec2_t b, struct rm_vec2_t *result);

/**
  @brief Calculates the magnitude (length) of a 2D vector
*/
enum reh_error_code_e rm_Vec2Magnitude(struct rm_vec2_t v, float *result);

/**
  @brief Normalizes a 2D vector (scales it to have a magnitude of 1)
*/
enum reh_error_code_e rm_Vec2Normalize(struct rm_vec2_t v, struct rm_vec2_t *n_v);

/**
  @brief Scales a 2D vector by a scalar value
*/
enum reh_error_code_e rm_Vec2Scale(struct rm_vec2_t v, float scalar, struct rm_vec2_t *result);

/**
  @brief Calculates the dot product of two 2D vectors
*/
enum reh_error_code_e rm_Vec2Dot(struct rm_vec2_t a, struct rm_vec2_t b, float *result);

/**
  @brief Calculates the distance between two 2D vectors
*/
enum reh_error_code_e rm_Vec2Distance(struct rm_vec2_t a, struct rm_vec2_t b, float *result);

#endif // VEC2_H
