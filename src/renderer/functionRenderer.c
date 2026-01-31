#include "renderer/functionRenderer.h"
#include "core/appContext.h"
#include "core/logger.h"
#include "core/errorHandler.h"
#include "core/window.h"
#include "expressionEngine/evaluator.h"
#include "expressionEngine/functionManager.h"
#include "utils/shaderUtils.h"
#include "math/utility.h"

#include <GLFW/glfw3.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

bool g_higherPrecisionRender = false;

enum reh_error_code_e rfr_Init(struct ra_app_context_t *context){
  if (context == nullptr){
    SET_ERROR_RETURN(ERR_INVALID_POINTER, "context structure passed to rfr_Init is NULL.");
  }

  char* vertexShaderSrc   = nullptr;
  char* fragmentShaderSrc = nullptr;

  CHECK_ERROR_CTX(rsu_LoadShaderSource("data/shaders/functionRender.vert", &vertexShaderSrc), "Failed to load vertex shader for the function renderer.");

  CHECK_ERROR_CTX(rsu_LoadShaderSource("data/shaders/basicColor.frag", &fragmentShaderSrc), "Failed to load fragment shader for the function renderer.");

  GLuint vertexShader = 0;
  GLuint fragShader = 0;
  context->fProgram = 0;

  enum reh_error_code_e _err = rsu_CompileShader(vertexShaderSrc, GL_VERTEX_SHADER, &vertexShader);
  if (_err != ERR_SUCCESS){
    free(vertexShaderSrc);
    free(fragmentShaderSrc);
    ADD_ERROR_CONTEXT_RETURN(_err, "Failed to compile vertex shader for the function renderer.");
  }

  _err = rsu_CompileShader(fragmentShaderSrc, GL_FRAGMENT_SHADER, &fragShader);
  if (_err != ERR_SUCCESS){
    free(vertexShaderSrc);
    free(fragmentShaderSrc);
    glDeleteShader(vertexShader);
    ADD_ERROR_CONTEXT_RETURN(_err, "Failed to compile fragment shader for the function renderer.");
  }

  _err = rsu_LinkShaders(vertexShader, fragShader, &context->fProgram);
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

enum reh_error_code_e rfr_SampleFunction(struct ree_function_t *function, float worldXRangeMin, float worldXRangeMax, float startStep, struct rfr_function_point_data_t *pointsData){
  if (function == nullptr){
    SET_ERROR_RETURN(ERR_INVALID_POINTER, "Function struct (ree_function_t) passed to rfr_SampleFunction is NULL.");
  }
  if (worldXRangeMin > worldXRangeMax){
    SET_ERROR_RETURN(ERR_INVALID_INPUT, "worldXRangeMin is bigger than worldXRangeMax (%f > %f) in rfr_SampleFunction.", (double)worldXRangeMin, (double)worldXRangeMax);
  }
  if (startStep <= 0){
    SET_ERROR_RETURN(ERR_INVALID_INPUT, "Invalid step provided to rfr_SampleFunction (%f)", (double)startStep);
  }
  if (pointsData == nullptr){
    SET_ERROR_RETURN(ERR_INVALID_POINTER, "vertices array passed to rfr_SampleFunction is NULL.");
  }

  // calculate samplecount
  float span = worldXRangeMax - worldXRangeMin;
  // estimate of size
  size_t sampleCount = (size_t)floorf(span / startStep) + 1;
  size_t sampleCapacity = sampleCount * 2;
  size_t sample = 0;

  // allocate memory for the vertices
  pointsData->vertices = (float *)malloc(sizeof(float) * sampleCapacity);

  if (pointsData->vertices == nullptr){
    SET_ERROR_RETURN(ERR_OUT_OF_MEMORY, "Failed to allocate memory for vertices in rfr_SampleFunction");
  }

  // allocate memory for undefined points
  size_t undefinedPointsCapacity = sampleCount;
  pointsData->undefinedPoints = (float *)malloc(sizeof(float) * undefinedPointsCapacity);
  pointsData->undefinedPointsCount = 0;
  if (pointsData->undefinedPoints == nullptr){
    SET_ERROR_RETURN(ERR_OUT_OF_MEMORY, "Failed to allocate memory for undefinedPoints in rfr_SampleFunction");
  }

  // sample the function
  float x = worldXRangeMin;
  float currentStep = startStep;

  // calculate the f0 once, then it gets set to f1 after each iteration
  // interestingly, if i calculate f0 each iteration, at the cost of more computational power, the tough functions can get rendered with more precision
  float f0; // left endpoint
  struct ree_variable_t variables[] = {{function->parameter, x}};
  enum reh_error_code_e _err = ree_EvaluateRpn(function->rpn, (size_t)function->rpnCount, variables, 1, &f0);
  RFR_CHECK_EVALUATOR_RETURN(_err, x);

  bool accepted = false;
  while (x < worldXRangeMax){
    // without using higher precision render, sampler acts odd (in stuff like x! or similar) but im lazy to try and fix this rn
    if (g_higherPrecisionRender == true){
      struct ree_variable_t variablesF0[] = {{function->parameter, x}};
      _err = ree_EvaluateRpn(function->rpn, (size_t)function->rpnCount, variablesF0, 1, &f0);
      RFR_CHECK_EVALUATOR_RETURN(_err, x);
    }

    float f1; // right endpoint
    float x1 = x + currentStep;
    struct ree_variable_t variablesNext[] = {{function->parameter, x1}};
    _err = ree_EvaluateRpn(function->rpn, (size_t)function->rpnCount, variablesNext, 1, &f1);
    RFR_CHECK_EVALUATOR_RETURN(_err, variablesNext[0].value);

    // linear interpolation
    float xm = x + currentStep * 0.5f;
    float fm; // midpoint
    variables[0].value = xm;
    _err = ree_EvaluateRpn(function->rpn, (size_t)function->rpnCount, variables, 1, &fm);
    RFR_CHECK_EVALUATOR_RETURN(_err, xm);

    // get error
    float err = fabsf(fm - ((f0 + f1) / (2.0f)));

    // determine whether we change the step
    {
      float maxAbs = RM_MAX_3(fabsf(f0), fabsf(f1), fabsf(fm));
      float threshold = (float)RFR_ABSOLUTE_TOLERANCE + (float)RFR_RELATIVE_TOLERANCE * maxAbs;
      accepted = err <= threshold;
    }

    if (accepted == false){
      if ((currentStep / 2.0f) < (startStep / RFR_BOTTOM_STEP_CAP)){
        RFR_CHECK_UNDEFINED_POINTS_MEMORY;
        // mark current point as undefined and advance
        pointsData->undefinedPoints[pointsData->undefinedPointsCount++] = x;
        currentStep = startStep;
        x += currentStep;
      }
      else currentStep /= 2.0f;
    }
    else {
      if ((sample + 2) > sampleCapacity){
        // we need to allocate more memory for vertices
        sampleCapacity *= 2;
        rl_LogMsg(RL_DEBUG, "Doubling memory for pointsData->vertices (Now at: %zu bytes)", sampleCapacity * sizeof(float));
        float* tmp = (float *)realloc(pointsData->vertices, sampleCapacity * sizeof(float));
        if (tmp == nullptr){
          free(pointsData->vertices);
          SET_ERROR_RETURN(ERR_OUT_OF_MEMORY, "Failed to reallocate memory for vertices data in sampler.");
        }
        else {
          pointsData->vertices = tmp;
        }
      }
      (pointsData->vertices)[sample++] = x;
      (pointsData->vertices)[sample++] = f0;
      f0 = f1;
      x += currentStep;
      currentStep *= 2;

      if (x >= worldXRangeMax){
        if ((sample + 2) > sampleCapacity){
          // we need to allocate space for one more point
          sampleCapacity += 2;
          float* tmp = (float *)realloc(pointsData->vertices, sampleCapacity * sizeof(float));
          if (tmp == nullptr){
            free(pointsData->vertices);
            SET_ERROR_RETURN(ERR_OUT_OF_MEMORY, "Failed to reallocate memory for vertices data in sampler.");
          }
          else {
            pointsData->vertices = tmp;
          }
        }
        // clamp x to the world limit and calculate y
        x = worldXRangeMax;
        struct ree_variable_t variables[] = {{function->parameter, x}};
        _err = ree_EvaluateRpn(function->rpn, (size_t)function->rpnCount, variables, 1, &f0);
        RFR_CHECK_EVALUATOR_RETURN(_err, variablesNext[0].value);

        (pointsData->vertices)[sample++] = x;
        (pointsData->vertices)[sample++] = f0;
      }
      accepted = true;
    }
  }

  pointsData->vertexCount = sample / 2;

  return ERR_SUCCESS;
}

enum reh_error_code_e rfr_Render(struct ra_app_context_t *context, struct ree_function_manager_t *functions, float **projectionMatrixPtr){
  if (context == nullptr){
    SET_ERROR_RETURN(ERR_INVALID_POINTER, "context passed to rfr_Render is NULL.");
  }
  else if (functions == nullptr){
    SET_ERROR_RETURN(ERR_INVALID_POINTER, "Function struct (ree_function_t) passed to rfr_Render is NULL.");
  }
  else if (projectionMatrixPtr == nullptr){
    SET_ERROR_RETURN(ERR_INVALID_POINTER, "Projection matrix pointer passed to rfr_Render is NULL.");
  }

  for (size_t i = 0; i < (size_t)functions->functionCount; ++i){
    struct ree_function_t *function = &functions->functions[i];

    // skip rendering functions that are not visible
    if (function->isVisible == false) continue;

    struct rfr_function_point_data_t pointData;

    // sample the function
    enum reh_error_code_e _err = rfr_SampleFunction(function, g_worldXMin, g_worldXMax, 0.001f, &pointData);
    if (_err != ERR_SUCCESS){
      if (_err == ERR_SAMPLER_STEP_TOO_SMALL){
        reh_ClearError();
        continue;
      }
      const struct reh_error_context_t *_ctx = reh_GetLastError();
      rl_LogError(_ctx, RL_ERROR);
      char _new_msg[256];
      snprintf(_new_msg, sizeof(_new_msg), "Failed to sample function.");
      reh_SetError(_err, __FILE__, __LINE__, __func__, _new_msg, reh_GetLastError()->message);
      return _err;
    }

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
    rsu_GluSetMat4(context->fProgram, "functionProjection", *projectionMatrixPtr);
    rsu_GluSet4f(context->fProgram, "color", function->color.x, function->color.y, function->color.z, 1.0f);
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
