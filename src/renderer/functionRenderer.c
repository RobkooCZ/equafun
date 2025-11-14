#include "renderer/functionRenderer.h"
#include "core/appContext.h"
#include "core/logger.h"
#include "core/errorHandler.h"
#include "core/window.h"
#include "expressionEngine/evaluator.h"
#include "expressionEngine/functionManager.h"
#include "utils/shaderUtils.h"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

enum reh_error_code_e rfr_init(struct ra_app_context_t *context){
  if (context == nullptr){
    SET_ERROR_RETURN(ERR_INVALID_POINTER, "context structure passed to rfr_init is NULL.");
  }

  char* vertexShaderSrc   = nullptr;
  char* fragmentShaderSrc = nullptr;

  CHECK_ERROR_CTX(loadShaderSource("data/shaders/functionRender.vert", &vertexShaderSrc), "Failed to load vertex shader for the function renderer.");

  CHECK_ERROR_CTX(loadShaderSource("data/shaders/basicColor.frag", &fragmentShaderSrc), "Failed to load fragment shader for the function renderer.");

  GLuint vertexShader = 0;
  GLuint fragShader = 0;
  context->fProgram = 0;

  enum reh_error_code_e _err = compileShader(vertexShaderSrc, GL_VERTEX_SHADER, &vertexShader);
  if (_err != ERR_SUCCESS){
    free(vertexShaderSrc);
    free(fragmentShaderSrc);
    ADD_ERROR_CONTEXT_RETURN(_err, "Failed to compile vertex shader for the function renderer.");
  }

  _err = compileShader(fragmentShaderSrc, GL_FRAGMENT_SHADER, &fragShader);
  if (_err != ERR_SUCCESS){
    free(vertexShaderSrc);
    free(fragmentShaderSrc);
    glDeleteShader(vertexShader);
    ADD_ERROR_CONTEXT_RETURN(_err, "Failed to compile fragment shader for the function renderer.");
  }

  _err = linkShaders(vertexShader, fragShader, &context->fProgram);
  free(vertexShaderSrc);
  free(fragmentShaderSrc);
  glDeleteShader(vertexShader);
  glDeleteShader(fragShader);

  if (_err != ERR_SUCCESS){
    ADD_ERROR_CONTEXT_RETURN(_err, "Failed to link shaders for the function renderer. program");
  }

  // Generate VAO
  glGenVertexArrays(1, &context->fVAO);
  GLenum err = glGetError();
  if (err != GL_NO_ERROR){
    char technical[256];
    snprintf(technical, sizeof(technical), "glGenVertexArrays failed with error: 0x%04X", err);
    glDeleteProgram(context->fProgram);
    context->fProgram = 0;
    SET_ERROR_TECHNICAL_RETURN(ERR_BUFFER_SETUP_FAILED, "Failed to generate Vertex Array Object", technical);
  }

  if (context->fVAO == 0){
    SET_ERROR_RETURN(ERR_INVALID_VAO, "Generated VAO is 0 (invalid)");
  }

  // Generate VBO
  glGenBuffers(1, &context->fVBO);
  err = glGetError();
  if (err != GL_NO_ERROR){
    char technical[256];
    snprintf(technical, sizeof(technical), "glGenBuffers(VBO) failed with error: 0x%04X", err);
    glDeleteVertexArrays(1, &context->fVAO);
    context->fVAO = 0;
    glDeleteProgram(context->fProgram);
    context->fProgram = 0;
    SET_ERROR_TECHNICAL_RETURN(ERR_BUFFER_SETUP_FAILED, "Failed to generate Vertex Buffer Object", technical);
  }

  if (context->fVBO == 0){
    glDeleteVertexArrays(1, &context->fVAO);
    context->fVAO = 0;
    SET_ERROR_RETURN(ERR_INVALID_VBO, "Generated VBO is 0 (invalid)");
  }

  // Bind context->fVAO
  glBindVertexArray(context->fVAO);
  err = glGetError();
  if (err != GL_NO_ERROR){
    char technical[256];
    snprintf(technical, sizeof(technical), "glBindVertexArray failed with error: 0x%04X", err);
    glDeleteBuffers(1, &context->fVBO);
    glDeleteVertexArrays(1, &context->fVAO);
    context->fVAO = 0;
    context->fVBO = 0;
    glDeleteProgram(context->fProgram);
    context->fProgram = 0;
    SET_ERROR_TECHNICAL_RETURN(ERR_BUFFER_SETUP_FAILED, "Failed to bind Vertex Array Object", technical);
  }

  // Bind and upload context->fVBO data
  glBindBuffer(GL_ARRAY_BUFFER, context->fVBO);
  err = glGetError();
  if (err != GL_NO_ERROR){
    char technical[256];
    snprintf(technical, sizeof(technical), "glBindBuffer(GL_ARRAY_BUFFER) failed with error: 0x%04X", err);
    glDeleteBuffers(1, &context->fVBO);
    glDeleteVertexArrays(1, &context->fVAO);
    context->fVAO = 0;
    context->fVBO = 0;
    glDeleteProgram(context->fProgram);
    context->fProgram = 0;
    SET_ERROR_TECHNICAL_RETURN(ERR_BUFFER_SETUP_FAILED, "Failed to bind Vertex Buffer Object", technical);
  }

  // set the initial capacity to 0, will be resized after sampling the function
  glBufferData(GL_ARRAY_BUFFER, 0, nullptr, GL_DYNAMIC_DRAW);
  context->fVertexCapacityBytes = 0;

  // vertex attribute pointer
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
  err = glGetError();
  if (err != GL_NO_ERROR){
    char technical[256];
    snprintf(technical, sizeof(technical), "glVertexAttribPointer failed with error: 0x%04X", err);
    glDeleteBuffers(1, &context->fVBO);
    glDeleteVertexArrays(1, &context->fVAO);
    context->fVAO = 0;
    context->fVBO = 0;
    glDeleteProgram(context->fProgram);
    context->fProgram = 0;
    SET_ERROR_TECHNICAL_RETURN(ERR_BUFFER_SETUP_FAILED, "Failed to set vertex attribute pointer", technical);
  }

  glEnableVertexAttribArray(0);
  err = glGetError();
  if (err != GL_NO_ERROR){
    char technical[256];
    snprintf(technical, sizeof(technical), "glEnableVertexAttribArray failed with error: 0x%04X", err);
    glDeleteBuffers(1, &context->fVBO);
    glDeleteVertexArrays(1, &context->fVAO);
    context->fVAO = 0;
    context->fVBO = 0;
    glDeleteProgram(context->fProgram);
    context->fProgram = 0;
    SET_ERROR_TECHNICAL_RETURN(ERR_BUFFER_SETUP_FAILED, "Failed to enable vertex attribute array", technical);
  }

  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);

  return ERR_SUCCESS;
}
enum reh_error_code_e rfr_sampleFunction(struct ree_function_t *function, float worldXRangeMin, float worldXRangeMax, float worldStep, struct rfr_function_point_data_t *pointsData){
  if (function == nullptr){
    SET_ERROR_RETURN(ERR_INVALID_POINTER, "Function struct (ree_function_t) passed to rfr_sampleFunction is NULL.");
  }
  if (worldXRangeMin > worldXRangeMax){
    SET_ERROR_RETURN(ERR_INVALID_INPUT, "worldXRangeMin is bigger than worldXRangeMax (%f > %f) in rfr_sampleFunction.", (double)worldXRangeMin, (double)worldXRangeMax);
  }
  if (worldStep <= 0){
    SET_ERROR_RETURN(ERR_INVALID_INPUT, "Invalid step provided to rfr_sampleFunction (%f)", (double)worldStep);
  }
  if (pointsData == nullptr ){
    SET_ERROR_RETURN(ERR_INVALID_POINTER, "vertices array passed to rfr_sampleFunction is NULL.");
  }

  // calculate samplecount
  float span = worldXRangeMax - worldXRangeMin;
  size_t sampleCount = (size_t)floorf(span / worldStep) + 1;
  size_t sample = 0;

  // allocate memory for the vertices
  pointsData->vertices = (float *)malloc(sizeof(float) * sampleCount * 2);

  if (pointsData->vertices == nullptr){
    SET_ERROR_RETURN(ERR_OUT_OF_MEMORY, "Failed to allocate memory for vertices in rfr_sampleFunction");
  }

  // allocate memory for undefined points
  pointsData->undefinedPoints = (float *)malloc(sizeof(float) * sampleCount);
  pointsData->undefinedPointsCount = 0;
  if (pointsData->undefinedPoints == nullptr){
    SET_ERROR_RETURN(ERR_OUT_OF_MEMORY, "Failed to allocate memory for undefinedPoints in rfr_sampleFunction");
  }

  // fill vertices (sample the function)
  for (size_t i = 0; i < sampleCount; ++i){
    float x = (float)(worldXRangeMin + (float)i * worldStep);
    // evaluate function at current x (get y)
    float y;
    struct ree_variable_t variables[] = {{function->parameter, x}};

    // gutted CHECK_ERROR_CTX macro
    enum reh_error_code_e _err = ree_evaluateRpn(function->rpn, (size_t)function->rpnCount, variables, 1, &y);

    if (!isfinite(y) || (fabsf(y) > ((worldYMax - worldYMin) * 10))){
        pointsData->undefinedPoints[pointsData->undefinedPointsCount++] = x;
        clearError();
        continue;
    }

    if (_err != ERR_SUCCESS){
      if (_err == ERR_DIVISION_BY_ZERO || _err == ERR_TAN_OUT_OF_DOMAIN ||
          _err == ERR_LOG_OUT_OF_DOMAIN || _err == ERR_LN_OUT_OF_DOMAIN ||
          _err == ERR_INVALID_SQRT){
        pointsData->undefinedPoints[pointsData->undefinedPointsCount++] = x;
        clearError();
        continue;
      }
      const ErrorContext *_ctx = getLastError();
      logError(_ctx, ERROR);
      char _new_msg[256];
      snprintf(_new_msg, sizeof(_new_msg), "Failed to evaluate RPN.");
      setError(_err, __FILE__, __LINE__, __func__, _new_msg, getLastError()->message);
      free(pointsData->vertices);
      pointsData->vertices = nullptr;
      pointsData->vertexCount = 0;
      return _err;
    }
    // add the coordinates into the vertex array
    (pointsData->vertices)[sample++] = x;
    (pointsData->vertices)[sample++] = y;
  }

  pointsData->vertexCount = sample / 2;

  return ERR_SUCCESS;
}

enum reh_error_code_e rfr_render(struct ra_app_context_t *context, struct ree_function_manager_t *functions, float **projectionMatrixPtr){
  if (context == nullptr){
    SET_ERROR_RETURN(ERR_INVALID_POINTER, "context passed to rfr_render is NULL.");
  }
  if (functions == nullptr){
    SET_ERROR_RETURN(ERR_INVALID_POINTER, "Function struct (ree_function_t) passed to rfr_render is NULL.");
  }
  for (size_t i = 0; i < (size_t)functions->functionCount; ++i){
    struct ree_function_t *function = &functions->functions[i];

    // skip rendering functions that are not visible
    if (function->isVisible == false) continue;

    struct rfr_function_point_data_t pointData;

    // sample the function
    CHECK_ERROR_CTX(rfr_sampleFunction(function, worldXMin, worldXMax, 0.01f, &pointData), "Failed to sample function.");

    // bind VAO, VBO and grow if needed
    glBindVertexArray(context->fVAO);
    glBindBuffer(GL_ARRAY_BUFFER, context->fVBO);

    const GLsizeiptr byteCount = (GLsizeiptr)(pointData.vertexCount * 2u * sizeof(float));
    // grow if the byte count is larger than the previous capacity
    if (byteCount > context->fVertexCapacityBytes){
      glBufferData(GL_ARRAY_BUFFER, byteCount, pointData.vertices, GL_DYNAMIC_DRAW);
      context->fVertexCapacityBytes = byteCount;
    }
    else {
      glBufferSubData(GL_ARRAY_BUFFER, 0, byteCount, pointData.vertices);
    }

    glUseProgram(context->fProgram);
    gluSetMat4(context->fProgram, "functionProjection", *projectionMatrixPtr);
    gluSet4f(context->fProgram, "color", function->color.x, function->color.y, function->color.z, 1.0f);
    glLineWidth(2.0f);

    size_t start = 0;
    size_t undefinedIndex = 0;

    while (start < pointData.vertexCount){
      // get the index of the undefined point
      while (undefinedIndex < pointData.undefinedPointsCount &&
          pointData.undefinedPoints[undefinedIndex] <= pointData.vertices[start * 2]
          ){
        ++undefinedIndex;
      }

      // first assume the end is the end of the array
      size_t end = pointData.vertexCount;

      if (undefinedIndex < pointData.undefinedPointsCount){
        float cutX = pointData.undefinedPoints[undefinedIndex];
        // try to find the end of this segment (between two undefined points, for example)
        for (size_t j = start; j < pointData.vertexCount; ++j){
          if (pointData.vertices[j * 2] > cutX){
            end = j;
            break;
          }
        }
      }

      // draw the array if the current segment has atleast two vertices
      if (end > start + 1){
        glDrawArrays(GL_LINE_STRIP, (GLint)start, (GLsizei)(end-start));
      }
      start = end;
    }

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    free(pointData.vertices);
    free(pointData.undefinedPoints);
  }
  return ERR_SUCCESS;
}
