#include "core/logger.h"
#include "glad/glad.h"
#include <GLFW/glfw3.h>

#include "renderer/graph.h"
#include "utils/renderUtils.h"
#include "utils/shaderUtils.h"
#include "core/errorHandler.h"

#include <stdio.h>
#include <stdlib.h>

enum ErrorCode setupGraph(GLuint *program, GLuint *VAO, GLuint *VBO, GLuint *EBO){
  if (!program || !VAO || !VBO || !EBO){
    SET_ERROR_RETURN(ERR_INVALID_POINTER, "One or more output pointers are NULL in setupGraph()");
  }

  char* vertexShaderSrc = nullptr;
  char* fragmentShaderSrc = nullptr;

  CHECK_ERROR_CTX(loadShaderSource("data/shaders/lineRender.vert", &vertexShaderSrc), "Failed to load vertex shader for graph axis");

  CHECK_ERROR_CTX(loadShaderSource("data/shaders/basicColor.frag", &fragmentShaderSrc), "Failed to load fragment shader for graph axis");

  GLuint vertexShader = 0;
  GLuint fragShader = 0;

  enum ErrorCode err = compileShader(vertexShaderSrc, GL_VERTEX_SHADER, &vertexShader);
  if (err != ERR_SUCCESS){
    free(vertexShaderSrc);
    free(fragmentShaderSrc);
    ADD_ERROR_CONTEXT_RETURN(err, "Failed to compile vertex shader for graph axis");
  }

  err = compileShader(fragmentShaderSrc, GL_FRAGMENT_SHADER, &fragShader);
  if (err != ERR_SUCCESS){
    free(vertexShaderSrc);
    free(fragmentShaderSrc);
    glDeleteShader(vertexShader);
    ADD_ERROR_CONTEXT_RETURN(err, "Failed to compile fragment shader for graph axis");
  }

  err = linkShaders(vertexShader, fragShader, program);
  free(vertexShaderSrc);
  free(fragmentShaderSrc);
  
  if (err != ERR_SUCCESS){
    ADD_ERROR_CONTEXT_RETURN(err, "Failed to link shaders for graph axis program");
  }

  float vertices[] = {
    // x axis
    X_MIN, 0.0f, 0.0f,
    X_MAX, 0.0f, 0.0f,

    // y axis
    0.0f, Y_MIN, 0.0f,
    0.0f, Y_MAX, 0.0f
  };

  GLuint indices[] = {
    0, 1, // x axis
    2, 3  // y axis
  };

  enum ErrorCode renderErr = setupRenderData(vertices, sizeof(vertices), indices, sizeof(indices), VAO, VBO, EBO);
  if (renderErr != ERR_SUCCESS){
    ADD_ERROR_CONTEXT_RETURN(renderErr, "Failed to setup render data for graph axis");
  }

  logMsg(SUCCESS, "Graph axis rendering initialized successfully");
  return ERR_SUCCESS;
}

enum ErrorCode renderGraph(GLuint *program, GLuint *VAO){
  if (!program || *program == 0){
    SET_ERROR_RETURN(ERR_RENDER_INVALID_PARAMS, "Invalid program in renderGraph()");
  }

  if (!VAO || *VAO == 0){
    SET_ERROR_RETURN(ERR_RENDER_INVALID_PARAMS, "Invalid VAO in renderGraph()");
  }

  glUseProgram(*program);
  glBindVertexArray(*VAO);
  gluSet4f(*program, "color", 1.0f, 1.0f, 1.0f, 1.0f);
  glLineWidth(2.0f);
  glDrawElements(GL_LINES, 4, GL_UNSIGNED_INT, 0);
  glBindVertexArray(0);

  return ERR_SUCCESS;
}

enum ErrorCode setupMarkers(GLuint *program, GLuint *VAO, GLuint *VBO, GLuint *EBO){
  if (!program || !VAO || !VBO || !EBO){
    SET_ERROR_RETURN(ERR_INVALID_POINTER, "One or more output pointers are NULL in setupMarkers()");
  }

  char* vertexShaderSrc = nullptr;
  char* fragmentShaderSrc = nullptr;

  CHECK_ERROR_CTX(loadShaderSource("data/shaders/lineRender.vert", &vertexShaderSrc), "Failed to load vertex shader for graph markers");

  CHECK_ERROR_CTX(loadShaderSource("data/shaders/basicColor.frag", &fragmentShaderSrc), "Failed to load fragment shader for graph markers");

  GLuint vertexShader = 0;
  GLuint fragShader = 0;

  enum ErrorCode err = compileShader(vertexShaderSrc, GL_VERTEX_SHADER, &vertexShader);
  if (err != ERR_SUCCESS){
    free(vertexShaderSrc);
    free(fragmentShaderSrc);
    ADD_ERROR_CONTEXT_RETURN(err, "Failed to compile vertex shader for graph markers");
  }

  err = compileShader(fragmentShaderSrc, GL_FRAGMENT_SHADER, &fragShader);
  if (err != ERR_SUCCESS){
    free(vertexShaderSrc);
    free(fragmentShaderSrc);
    glDeleteShader(vertexShader);
    ADD_ERROR_CONTEXT_RETURN(err, "Failed to compile fragment shader for graph markers");
  }

  err = linkShaders(vertexShader, fragShader, program);
  free(vertexShaderSrc);
  free(fragmentShaderSrc);
  
  if (err != ERR_SUCCESS){
    ADD_ERROR_CONTEXT_RETURN(err, "Failed to link shaders for graph marker program");
  }

  float step = POINT_DISTANCE;
  float vertices[(X_COUNT + Y_COUNT) * 3 * 2];

  int v = 0;
  for (int i = 0; i < X_COUNT; ++i){
    float x = X_MIN + i * step;
    vertices[v++] = x;
    vertices[v++] = 0.0f + POINT_MARKER_HEIGHT;
    vertices[v++] = 0.0f;
    
    vertices[v++] = x;
    vertices[v++] = 0.0f - POINT_MARKER_HEIGHT;
    vertices[v++] = 0.0f;
  }

  for (int i = 0; i < Y_COUNT; ++i){
    float y = Y_MIN + i * step;
    vertices[v++] = 0.0f + POINT_MARKER_HEIGHT;
    vertices[v++] = y;
    vertices[v++] = 0.0f;
    
    vertices[v++] = 0.0f - POINT_MARKER_HEIGHT;
    vertices[v++] = y;
    vertices[v++] = 0.0f;
  }

  GLuint indices[TOTAL_MARKERS * 2];
  for (int k = 0; k < TOTAL_MARKERS * 2; ++k){
    indices[k] = k;
  }

  enum ErrorCode renderErr = setupRenderData(vertices, sizeof(vertices), indices, sizeof(indices), VAO, VBO, EBO);
  if (renderErr != ERR_SUCCESS){
    ADD_ERROR_CONTEXT_RETURN(renderErr, "Failed to setup render data for graph markers");
  }

  logMsg(SUCCESS, "Graph marker rendering initialized successfully");
  return ERR_SUCCESS;
}

enum ErrorCode renderMarkers(GLuint *program, GLuint *VAO){
  if (!program || *program == 0){
    SET_ERROR_RETURN(ERR_RENDER_INVALID_PARAMS, "Invalid program in renderMarkers()");
  }

  if (!VAO || *VAO == 0){
    SET_ERROR_RETURN(ERR_RENDER_INVALID_PARAMS, "Invalid VAO in renderMarkers()");
  }

  int indexCount = TOTAL_MARKERS * 2;
  
  glUseProgram(*program);
  glBindVertexArray(*VAO);
  gluSet4f(*program, "color", 1.0f, 1.0f, 1.0f, 1.0f);
  glLineWidth(2.0f);
  glDrawElements(GL_LINES, indexCount, GL_UNSIGNED_INT, 0);
  glBindVertexArray(0);

  return ERR_SUCCESS;
}
