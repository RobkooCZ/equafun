#include "core/logger.h"
#include "glad/glad.h"
#include <GLFW/glfw3.h>

#include "renderer/graph.h"
#include "utils/renderUtils.h"
#include "utils/shaderUtils.h"
#include "core/errorHandler.h"

#include <stdlib.h>

enum reh_error_code_e setupGraph(GLuint *program, GLuint *VAO, GLuint *VBO, GLuint *EBO){
  if (!program || !VAO || !VBO || !EBO){
    SET_ERROR_RETURN(ERR_INVALID_POINTER, "One or more output pointers are NULL in setupGraph()");
  }

  char* vertexShaderSrc = nullptr;
  char* fragmentShaderSrc = nullptr;

  CHECK_ERROR_CTX(loadShaderSource("data/shaders/lineRender.vert", &vertexShaderSrc), "Failed to load vertex shader for graph axis");

  CHECK_ERROR_CTX(loadShaderSource("data/shaders/basicColor.frag", &fragmentShaderSrc), "Failed to load fragment shader for graph axis");

  GLuint vertexShader = 0;
  GLuint fragShader = 0;

  enum reh_error_code_e err = compileShader(vertexShaderSrc, GL_VERTEX_SHADER, &vertexShader);
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
    -1.0f, 0.0f, 0.0f,
     1.0f, 0.0f, 0.0f,

    // y axis
    0.0f, -1.0f, 0.0f,
    0.0f,  1.0f, 0.0f
  };

  GLuint indices[] = {
    0, 1, // x axis
    2, 3  // y axis
  };

  enum reh_error_code_e renderErr = setupRenderData(vertices, sizeof(vertices), indices, sizeof(indices), VAO, VBO, EBO);
  if (renderErr != ERR_SUCCESS){
    ADD_ERROR_CONTEXT_RETURN(renderErr, "Failed to setup render data for graph axis");
  }

  logMsg(SUCCESS, "Graph axis rendering initialized successfully");
  return ERR_SUCCESS;
}

enum reh_error_code_e renderGraph(GLuint *program, GLuint *VAO){
  if (program == nullptr || *program == 0){
    SET_ERROR_RETURN(ERR_RENDER_INVALID_PARAMS, "Invalid program in renderGraph()");
  }

  if (VAO == nullptr || *VAO == 0){
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


enum reh_error_code_e setupMarkerShaders(GLuint *program){
  if (!program){
    SET_ERROR_RETURN(ERR_INVALID_POINTER, "Program pointer is NULL in setupMarkerShaders()");
  }

  char* vertexShaderSrc = nullptr;
  char* fragmentShaderSrc = nullptr;

  CHECK_ERROR_CTX(loadShaderSource("data/shaders/lineRender.vert", &vertexShaderSrc), "Failed to load vertex shader for graph markers");
  CHECK_ERROR_CTX(loadShaderSource("data/shaders/basicColor.frag", &fragmentShaderSrc), "Failed to load fragment shader for graph markers");

  GLuint vertexShader = 0;
  GLuint fragShader = 0;

  enum reh_error_code_e err = compileShader(vertexShaderSrc, GL_VERTEX_SHADER, &vertexShader);
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

  logMsg(SUCCESS, "Graph marker shaders initialized successfully");
  return ERR_SUCCESS;
}

enum reh_error_code_e setupMarkerBuffers(GLuint *VAO, GLuint *VBO, GLuint *EBO){
  if (!VAO || !VBO || !EBO){
    SET_ERROR_RETURN(ERR_INVALID_POINTER, "One or more output pointers are NULL in setupMarkerBuffers()");
  }

  // Generate buffers
  glGenVertexArrays(1, VAO);
  glGenBuffers(1, VBO);
  glGenBuffers(1, EBO);

  glBindVertexArray(*VAO);

  // Allocate empty VBO with GL_DYNAMIC_DRAW (will be updated each frame)
  glBindBuffer(GL_ARRAY_BUFFER, *VBO);
  glBufferData(GL_ARRAY_BUFFER, 0, NULL, GL_DYNAMIC_DRAW);

  // Set vertex attribute pointer
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(0);

  // Allocate empty EBO
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, *EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, 0, NULL, GL_DYNAMIC_DRAW);

  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);

  logMsg(SUCCESS, "Graph marker buffers initialized successfully");
  return ERR_SUCCESS;
}

enum reh_error_code_e renderMarkers(GLuint *program, GLuint *VAO, GLuint *VBO, GLuint *EBO){
  if (!program || *program == 0){
    SET_ERROR_RETURN(ERR_RENDER_INVALID_PARAMS, "Invalid program in renderMarkers()");
  }

  if (!VAO || *VAO == 0 || !VBO || *VBO == 0 || !EBO || *EBO == 0){
    SET_ERROR_RETURN(ERR_RENDER_INVALID_PARAMS, "Invalid VAO/VBO/EBO in renderMarkers()");
  }

  // calculate visible bounds (for now, im just using NDC -1 to 1)
  float leftEdge   = -1.0f;
  float rightEdge  =  1.0f;
  float bottomEdge = -1.0f;
  float topEdge    =  1.0f;

  float gridSpacingNDC = GRID_SPACING_NDC;

  // Count how many markers we need
  int xMarkerCount = 0;
  for (float x = 0.0f; x <= rightEdge; x += gridSpacingNDC) xMarkerCount++;
  for (float x = -gridSpacingNDC; x >= leftEdge; x -= gridSpacingNDC) xMarkerCount++;

  int yMarkerCount = 0;
  for (float y = 0.0f; y <= topEdge; y += gridSpacingNDC) yMarkerCount++;
  for (float y = -gridSpacingNDC; y >= bottomEdge; y -= gridSpacingNDC) yMarkerCount++;

  int totalMarkers = xMarkerCount + yMarkerCount;
  int vertexCount = totalMarkers * 2; // 2 vertices per marker (top and bottom of tick)

  // Allocate arrays
  float *vertices = malloc(vertexCount * 3 * sizeof(float));
  GLuint *indices = malloc(vertexCount * sizeof(GLuint));

  if (vertices == nullptr || indices == nullptr){
    free(vertices);
    free(indices);
    SET_ERROR_RETURN(ERR_OUT_OF_MEMORY, "Failed to allocate marker vertex/index data");
  }

  // fill arrays
  int v = 0;
  int idx = 0;
  int indicesVal = 0;

  // X-axis markers (positive)
  for (float x = 0.0f; x <= rightEdge; x += gridSpacingNDC){
    vertices[v++] = x;
    vertices[v++] = 0.0f + POINT_MARKER_HEIGHT;
    vertices[v++] = 0.0f;

    vertices[v++] = x;
    vertices[v++] = 0.0f - POINT_MARKER_HEIGHT;
    vertices[v++] = 0.0f;

    indices[idx++] = indicesVal++;
    indices[idx++] = indicesVal++;
  }

  // X-axis markers (negative)
  for (float x = -gridSpacingNDC; x >= leftEdge; x -= gridSpacingNDC){
    vertices[v++] = x;
    vertices[v++] = 0.0f + POINT_MARKER_HEIGHT;
    vertices[v++] = 0.0f;

    vertices[v++] = x;
    vertices[v++] = 0.0f - POINT_MARKER_HEIGHT;
    vertices[v++] = 0.0f;

    indices[idx++] = indicesVal++;
    indices[idx++] = indicesVal++;
  }

  // Y-axis markers (positive)
  for (float y = 0.0f; y <= topEdge; y += gridSpacingNDC){
    vertices[v++] = 0.0f + POINT_MARKER_HEIGHT;
    vertices[v++] = y;
    vertices[v++] = 0.0f;

    vertices[v++] = 0.0f - POINT_MARKER_HEIGHT;
    vertices[v++] = y;
    vertices[v++] = 0.0f;

    indices[idx++] = indicesVal++;
    indices[idx++] = indicesVal++;
  }

  // Y-axis markers (negative)
  for (float y = -gridSpacingNDC; y >= bottomEdge; y -= gridSpacingNDC){
    vertices[v++] = 0.0f + POINT_MARKER_HEIGHT;
    vertices[v++] = y;
    vertices[v++] = 0.0f;

    vertices[v++] = 0.0f - POINT_MARKER_HEIGHT;
    vertices[v++] = y;
    vertices[v++] = 0.0f;

    indices[idx++] = indicesVal++;
    indices[idx++] = indicesVal++;
  }

  // Update VBO and EBO
  glBindVertexArray(*VAO);

  glBindBuffer(GL_ARRAY_BUFFER, *VBO);
  glBufferData(GL_ARRAY_BUFFER, vertexCount * 3 * sizeof(float), vertices, GL_DYNAMIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, *EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, vertexCount * sizeof(GLuint), indices, GL_DYNAMIC_DRAW);

  // Render
  glUseProgram(*program);
  gluSet4f(*program, "color", 1.0f, 1.0f, 1.0f, 1.0f);
  glLineWidth(2.0f);
  glDrawElements(GL_LINES, vertexCount, GL_UNSIGNED_INT, 0);

  glBindVertexArray(0);

  // Cleanup
  free(vertices);
  free(indices);

  return ERR_SUCCESS;
}
