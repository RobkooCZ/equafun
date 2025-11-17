/*
  rfr - Robkoo's Function Renderer
*/

#ifndef FUNCTION_RENDERER_H
#define FUNCTION_RENDERER_H

#include "core/appContext.h"
#include "expressionEngine/functionManager.h"
#include "core/errorHandler.h"

struct rfr_function_point_data_t {
  float *vertices;                /**< Array of vertex positions */
  size_t vertexCount;             /**< Number of vertices */
  float *undefinedPoints;         /**< Array of undefined point positions */
  size_t undefinedPointsCount;    /**< Number of undefined points */
};

/**
  @brief Initializes the function renderer
*/
enum reh_error_code_e rfr_Init(struct ra_app_context_t *context);

/**
  @brief Samples a function over a specified range and step size
*/
enum reh_error_code_e rfr_SampleFunction(struct ree_function_t *function, float worldXRangeMin, float worldXRangeMax, float worldStep, struct rfr_function_point_data_t *pointsData);

/**
  @brief Renders the sampled function points
*/
enum reh_error_code_e rfr_Render(struct ra_app_context_t *context, struct ree_function_manager_t *functions, float **projectionMatrixPtr);

#endif//FUNCTION_RENDERER_H