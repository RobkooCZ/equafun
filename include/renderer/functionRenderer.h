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


extern bool g_higherPrecisionRender;

#define RFR_ABSOLUTE_TOLERANCE 1e-2
#define RFR_RELATIVE_TOLERANCE 1e-3

#define RFR_BOTTOM_STEP_CAP     1024 // times smaller than the input step
#define RFR_TOP_STEP_CAP        16  // times bigger than the input step

#define RFR_CHECK_UNDEFINED_POINTS_MEMORY \
  do { \
    if (pointsData->undefinedPointsCount + 1 > undefinedPointsCapacity) { \
      undefinedPointsCapacity *= 2; \
      rl_LogMsg(RL_DEBUG, "Doubling memory for pointsData->undefinedPointsCount (Now at: %zu bytes)", undefinedPointsCapacity * sizeof(float)); \
      float *tmp = (float *)realloc(pointsData->undefinedPoints, undefinedPointsCapacity * sizeof(float)); \
      if (tmp == nullptr){ \
        free(pointsData->undefinedPoints); \
        SET_ERROR_RETURN(ERR_OUT_OF_MEMORY, "Failed to reallocate memory for undefined points in sampler."); \
      } \
      else { \
        pointsData->undefinedPoints = tmp; \
      } \
    } \
  } while(0)

#define RFR_CHECK_EVALUATOR_RETURN(_err, xUndef) \
do { \
  if (_err != ERR_SUCCESS){ \
      if (_err == ERR_DIVISION_BY_ZERO || _err == ERR_TAN_OUT_OF_DOMAIN || \
          _err == ERR_LOG_OUT_OF_DOMAIN || _err == ERR_LN_OUT_OF_DOMAIN || \
          _err == ERR_INVALID_SQRT){ \
        RFR_CHECK_UNDEFINED_POINTS_MEMORY; \
        pointsData->undefinedPoints[pointsData->undefinedPointsCount++] = xUndef; \
        x += currentStep; \
        reh_ClearError(); \
        continue; \
      } \
      const struct reh_error_context_t *_ctx = reh_GetLastError(); \
      rl_LogError(_ctx, RL_ERROR); \
      char _new_msg[256]; \
      snprintf(_new_msg, sizeof(_new_msg), "Failed to evaluate RPN."); \
      reh_SetError(_err, __FILE__, __LINE__, __func__, _new_msg, reh_GetLastError()->message); \
      free(pointsData->vertices); \
      pointsData->vertices = nullptr; \
      pointsData->vertexCount = 0; \
      return _err; \
    } \
} while(0)

/**
  @brief Initializes the function renderer
*/
enum reh_error_code_e rfr_Init(struct ra_app_context_t *context);

/**
  @brief Samples a function over a specified range and step size
*/
enum reh_error_code_e rfr_SampleFunction(struct ree_function_t *function, float worldXRangeMin, float worldXRangeMax, float startStep, struct rfr_function_point_data_t *pointsData);

/**
  @brief Renders the sampled function points
*/
enum reh_error_code_e rfr_Render(struct ra_app_context_t *context, struct ree_function_manager_t *functions, float **projectionMatrixPtr);

#endif//FUNCTION_RENDERER_H
