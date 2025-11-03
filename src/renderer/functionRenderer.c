#include "renderer/functionRenderer.h"
#include "core/appContext.h"
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

enum reh_error_code_e rfr_sampleFunction(struct ree_function_t *function, float worldXRangeMin, float worldXRangeMax, float worldStep, float **vertices, size_t *outVertexCount){
  if (function == nullptr){
    SET_ERROR_RETURN(ERR_INVALID_POINTER, "Function struct (ree_function_t) passed to rfr_sampleFunction is NULL.");
  }
  if (worldXRangeMin > worldXRangeMax){
    SET_ERROR_RETURN(ERR_INVALID_INPUT, "worldXRangeMin is bigger than worldXRangeMax (%f > %f) in rfr_sampleFunction.", worldXRangeMin, worldXRangeMax);
  }
  if (worldStep <= 0){
    SET_ERROR_RETURN(ERR_INVALID_INPUT, "Invalid step provided to rfr_sampleFunction (%f)", worldStep);
  }
  if (vertices == nullptr){
    SET_ERROR_RETURN(ERR_INVALID_POINTER, "vertices array passed to rfr_sampleFunction is NULL.");
  }
  if (outVertexCount == nullptr){
    SET_ERROR_RETURN(ERR_INVALID_POINTER, "outVertexCount passed to rfr_sampleFunction is NULL.");
  }

  // calculate samplecount
  float span = worldXRangeMax - worldXRangeMin;
  size_t sampleCount = (size_t)floorf(span / worldStep) + 1;
  size_t sample = 0;

  // allocate memory for the vertices
  *vertices = (float *)malloc(sizeof(float) * sampleCount * 2);

  if (*vertices == nullptr){
    SET_ERROR_RETURN(ERR_OUT_OF_MEMORY, "Failed to allocate memory for vertices in rfr_sampleFunction");
  }

  // fill vertices (sample the function)
  for (size_t i = 0; i < sampleCount; ++i){
    float x = worldXRangeMin + i * worldStep;
    // evaluate function at current x (get y)
    float y;
    struct ree_variable_t variables[] = {{"x", x}};

    // gutted CHECK_ERROR_CTX macro
    enum reh_error_code_e _err = ree_evaluateRpn(function->rpn, (size_t)function->rpnCount, variables, 1, &y);
    if (_err != ERR_SUCCESS){
      if (_err == ERR_DIVISION_BY_ZERO || _err == ERR_TAN_OUT_OF_DOMAIN ||
          _err == ERR_LOG_OUT_OF_DOMAIN || _err == ERR_LN_OUT_OF_DOMAIN ||
          _err == ERR_INVALID_SQRT){
        clearError();
        continue;
      }
      printf("tech det: %s\n", getLastError()->message);
      char _new_msg[256];
      snprintf(_new_msg, sizeof(_new_msg), "Failed to evaluate RPN.");
      setError(_err, __FILE__, __LINE__, __func__, _new_msg, getLastError()->message);
      free(*vertices);
      *vertices = nullptr;
      *outVertexCount = 0;
      return _err;
    }
    // add the coordinates into the vertex array
    (*vertices)[sample++] = x;
    (*vertices)[sample++] = y;
  }

  *outVertexCount = sample / 2;

  return ERR_SUCCESS;
}
enum reh_error_code_e rfr_render(struct ra_app_context_t *context, struct ree_function_manager_t *functions, float **projectionMatrixPtr){
  if (context == nullptr){
    SET_ERROR_RETURN(ERR_INVALID_POINTER, "context passed to rfr_render is NULL.");
  }
  if (functions == nullptr){
    SET_ERROR_RETURN(ERR_INVALID_POINTER, "Function struct (ree_function_t) passed to rfr_render is NULL.");
  }
  for (size_t i = 0; i < functions->functionCount; ++i){
    struct ree_function_t *function = &functions->functions[i];

    // skip rendering functions that are not visible
    if (function->isVisible == false) continue;

    float *vertices;
    size_t vertexCount = 0;
    // sample the function
    enum reh_error_code_e _err = rfr_sampleFunction(function, worldXMin, worldXMax, 0.02f, &vertices, &vertexCount);
    if (_err != ERR_SUCCESS){
      printf("Error code %d\n", getLastError()->code);
      SET_ERROR_TECHNICAL_RETURN(_err, "Failed to sample function", getLastError()->technicalDetails);
    }
    // bind VAO, VBO and grow if needed
    glBindVertexArray(context->fVAO);
    glBindBuffer(GL_ARRAY_BUFFER, context->fVBO);

    const GLsizeiptr byteCount = (GLsizeiptr)(vertexCount * 2u * sizeof(float));
    // grow if the byte count is larger than the previous capacity
    if (byteCount > context->fVertexCapacityBytes){
      glBufferData(GL_ARRAY_BUFFER, byteCount, vertices, GL_DYNAMIC_DRAW);
      context->fVertexCapacityBytes = byteCount;
    }
    else {
      glBufferSubData(GL_ARRAY_BUFFER, 0, byteCount, vertices);
    }

    glUseProgram(context->fProgram);
    gluSetMat4(context->fProgram, "functionProjection", *projectionMatrixPtr);
    gluSet4f(context->fProgram, "color", function->color.x, function->color.y, function->color.z, 1.0f);
    glLineWidth(2.0f);
    glDrawArrays(GL_LINE_STRIP, 0, vertexCount);
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    free(vertices);
  }
  return ERR_SUCCESS;
}
