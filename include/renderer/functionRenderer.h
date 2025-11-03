#ifndef FUNCTION_RENDERER_H
#define FUNCTION_RENDERER_H

#include "core/appContext.h"
#include "expressionEngine/functionManager.h"
#include "core/errorHandler.h"

enum reh_error_code_e rfr_init(struct ra_app_context_t *context);
enum reh_error_code_e rfr_sampleFunction(struct ree_function_t *function, float worldXRangeMin, float worldXRangeMax, float worldStep, float **vertices, size_t *outVertexCount);
enum reh_error_code_e rfr_render(struct ra_app_context_t *context, struct ree_function_manager_t *functions, float **projectionMatrixPtr);

#endif//FUNCTION_RENDERER_H
