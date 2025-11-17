#include "core/logger.h"
#include "core/window.h"
#include "glad/glad.h"
#include <GLFW/glfw3.h>

#include "renderer/graph.h"
#include "utils/renderUtils.h"
#include "utils/shaderUtils.h"
#include "core/errorHandler.h"

#include <stdlib.h>

enum reh_error_code_e rgr_SetupGraph(GLuint *program, GLuint *VAO, GLuint *VBO, GLuint *EBO){
  if (!program || !VAO || !VBO || !EBO){
    SET_ERROR_RETURN(ERR_INVALID_POINTER, "One or more output pointers are NULL in rgr_SetupGraph()");
  }

  char* vertexShaderSrc = nullptr;
  char* fragmentShaderSrc = nullptr;

  CHECK_ERROR_CTX(rsu_LoadShaderSource("data/shaders/lineRender.vert", &vertexShaderSrc), "Failed to load vertex shader for graph axis");

  CHECK_ERROR_CTX(rsu_LoadShaderSource("data/shaders/basicColor.frag", &fragmentShaderSrc), "Failed to load fragment shader for graph axis");

  GLuint vertexShader = 0;
  GLuint fragShader = 0;

  enum reh_error_code_e err = rsu_CompileShader(vertexShaderSrc, GL_VERTEX_SHADER, &vertexShader);
  if (err != ERR_SUCCESS){
    free(vertexShaderSrc);
    free(fragmentShaderSrc);
    ADD_ERROR_CONTEXT_RETURN(err, "Failed to compile vertex shader for graph axis");
  }

  err = rsu_CompileShader(fragmentShaderSrc, GL_FRAGMENT_SHADER, &fragShader);
  if (err != ERR_SUCCESS){
    free(vertexShaderSrc);
    free(fragmentShaderSrc);
    glDeleteShader(vertexShader);
    ADD_ERROR_CONTEXT_RETURN(err, "Failed to compile fragment shader for graph axis");
  }

  err = rsu_LinkShaders(vertexShader, fragShader, program);
  free(vertexShaderSrc);
  free(fragmentShaderSrc);

  if (err != ERR_SUCCESS){
    ADD_ERROR_CONTEXT_RETURN(err, "Failed to link shaders for graph axis program");
  }

  float vertices[] = {
    // x axis
    worldXMin, 0.0f, 0.0f,
    worldXMax, 0.0f, 0.0f,

    // y axis
    0.0f, worldYMin, 0.0f,
    0.0f, worldYMax, 0.0f
  };

  GLuint indices[] = {
    0, 1, // x axis
    2, 3  // y axis
  };

  enum reh_error_code_e renderErr = rru_SetupRenderData(vertices, sizeof(vertices), indices, sizeof(indices), VAO, VBO, EBO);
  if (renderErr != ERR_SUCCESS){
    ADD_ERROR_CONTEXT_RETURN(renderErr, "Failed to setup render data for graph axis");
  }

  rl_LogMsg(RL_SUCCESS, "Graph axis rendering initialized successfully");
  return ERR_SUCCESS;
}

enum reh_error_code_e rgr_RenderGraph(GLuint *program, GLuint *VAO, GLuint *VBO, float **projectionMatrixPtr){
  if (program == nullptr || *program == 0){
    SET_ERROR_RETURN(ERR_RRL_ENDER_INVALID_PARAMS, "Invalid program in rgr_RenderGraph()");
  }

  if (VAO == nullptr || *VAO == 0){
    SET_ERROR_RETURN(ERR_RRL_ENDER_INVALID_PARAMS, "Invalid VAO in rgr_RenderGraph()");
  }

  // get new graph vertices again as the resolution might've changed
  float vertices[] = {
    worldXMin, 0.0f, 0.0f,
    worldXMax, 0.0f, 0.0f,
    0.0f,      worldYMin, 0.0f,
    0.0f,      worldYMax, 0.0f
  };

  glBindBuffer(GL_ARRAY_BUFFER, *VBO);
  glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  glUseProgram(*program);
  rsu_GluSetMat4(*program, "graphProjection", *projectionMatrixPtr);
  glBindVertexArray(*VAO);
  rsu_GluSet4f(*program, "color", 1.0f, 1.0f, 1.0f, 1.0f);
  glLineWidth(2.0f);
  glDrawElements(GL_LINES, 4, GL_UNSIGNED_INT, 0);
  glBindVertexArray(0);

  return ERR_SUCCESS;
}


enum reh_error_code_e rgr_SetupMarkerShaders(GLuint *program){
  if (!program){
    SET_ERROR_RETURN(ERR_INVALID_POINTER, "Program pointer is NULL in rgr_SetupMarkerShaders()");
  }

  char* vertexShaderSrc = nullptr;
  char* fragmentShaderSrc = nullptr;

  CHECK_ERROR_CTX(rsu_LoadShaderSource("data/shaders/lineRender.vert", &vertexShaderSrc), "Failed to load vertex shader for graph markers");
  CHECK_ERROR_CTX(rsu_LoadShaderSource("data/shaders/basicColor.frag", &fragmentShaderSrc), "Failed to load fragment shader for graph markers");

  GLuint vertexShader = 0;
  GLuint fragShader = 0;

  enum reh_error_code_e err = rsu_CompileShader(vertexShaderSrc, GL_VERTEX_SHADER, &vertexShader);
  if (err != ERR_SUCCESS){
    free(vertexShaderSrc);
    free(fragmentShaderSrc);
    ADD_ERROR_CONTEXT_RETURN(err, "Failed to compile vertex shader for graph markers");
  }

  err = rsu_CompileShader(fragmentShaderSrc, GL_FRAGMENT_SHADER, &fragShader);
  if (err != ERR_SUCCESS){
    free(vertexShaderSrc);
    free(fragmentShaderSrc);
    glDeleteShader(vertexShader);
    ADD_ERROR_CONTEXT_RETURN(err, "Failed to compile fragment shader for graph markers");
  }

  err = rsu_LinkShaders(vertexShader, fragShader, program);
  free(vertexShaderSrc);
  free(fragmentShaderSrc);
  
  if (err != ERR_SUCCESS){
    ADD_ERROR_CONTEXT_RETURN(err, "Failed to link shaders for graph marker program");
  }

  rl_LogMsg(RL_SUCCESS, "Graph marker shaders initialized successfully");
  return ERR_SUCCESS;
}

enum reh_error_code_e rgr_SetupMarkerBuffers(GLuint *VAO, GLuint *VBO, GLuint *EBO){
  if (!VAO || !VBO || !EBO){
    SET_ERROR_RETURN(ERR_INVALID_POINTER, "One or more output pointers are NULL in rgr_SetupMarkerBuffers()");
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

  rl_LogMsg(RL_SUCCESS, "Graph marker buffers initialized successfully");
  return ERR_SUCCESS;
}

enum reh_error_code_e rgr_RenderMarkers(GLuint *program, GLuint *VAO, GLuint *VBO, GLuint *EBO, float **projectionMatrixPtr){
  if (!program || *program == 0){
    SET_ERROR_RETURN(ERR_RRL_ENDER_INVALID_PARAMS, "Invalid program in rgr_RenderMarkers()");
  }

  if (!VAO || *VAO == 0 || !VBO || *VBO == 0 || !EBO || *EBO == 0){
    SET_ERROR_RETURN(ERR_RRL_ENDER_INVALID_PARAMS, "Invalid VAO/VBO/EBO in rgr_RenderMarkers()");
  }

  // set useful vars
  float gridSpacingWorld = GRID_SPACING_WORLD;
  float pointMarkerHeight = POINT_MARKER_HEIGHT_WORLD;

  // set padding
  float horizontalPadding = pointMarkerHeight;
  float verticalPadding   = pointMarkerHeight;

  // get the world coordinates with the padding
  float leftEdge   = worldXMin + horizontalPadding;
  float rightEdge  = worldXMax - horizontalPadding;
  float bottomEdge = worldYMin + verticalPadding;
  float topEdge    = worldYMax - verticalPadding;

  // Count how many markers we need
  int xMarkerCount = 0;
  for (float x = 0.0f; x <= rightEdge; x += gridSpacingWorld) xMarkerCount++;
  for (float x = -gridSpacingWorld; x >= leftEdge; x -= gridSpacingWorld) xMarkerCount++;

  int yMarkerCount = 0;
  for (float y = 0.0f; y <= topEdge; y += gridSpacingWorld) yMarkerCount++;
  for (float y = -gridSpacingWorld; y >= bottomEdge; y -= gridSpacingWorld) yMarkerCount++;

  int totalMarkers = xMarkerCount + yMarkerCount;
  int vertexCount = totalMarkers * 2; // 2 vertices per marker (top and bottom of tick)

  // Allocate arrays
  float *vertices = malloc((long unsigned int)vertexCount * (long unsigned int)3 * sizeof(float));
  GLuint *indices = malloc((long unsigned int)vertexCount * sizeof(GLuint));

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
  for (float x = 0.0f; x <= rightEdge; x += gridSpacingWorld){
    vertices[v++] = x;
    vertices[v++] = 0.0f + pointMarkerHeight;
    vertices[v++] = 0.0f;

    vertices[v++] = x;
    vertices[v++] = 0.0f - pointMarkerHeight;
    vertices[v++] = 0.0f;

    indices[idx++] = (GLuint)indicesVal++;
    indices[idx++] = (GLuint)indicesVal++;
  }

  // X-axis markers (negative)
  for (float x = -gridSpacingWorld; x >= leftEdge; x -= gridSpacingWorld){
    vertices[v++] = x;
    vertices[v++] = 0.0f + pointMarkerHeight;
    vertices[v++] = 0.0f;

    vertices[v++] = x;
    vertices[v++] = 0.0f - pointMarkerHeight;
    vertices[v++] = 0.0f;

    indices[idx++] = (GLuint)indicesVal++;
    indices[idx++] = (GLuint)indicesVal++;
  }

  // Y-axis markers (positive)
  for (float y = 0.0f; y <= topEdge; y += gridSpacingWorld){
    vertices[v++] = 0.0f + pointMarkerHeight;
    vertices[v++] = y;
    vertices[v++] = 0.0f;

    vertices[v++] = 0.0f - pointMarkerHeight;
    vertices[v++] = y;
    vertices[v++] = 0.0f;

    indices[idx++] = (GLuint)indicesVal++;
    indices[idx++] = (GLuint)indicesVal++;
  }

  // Y-axis markers (negative)
  for (float y = -gridSpacingWorld; y >= bottomEdge; y -= gridSpacingWorld){
    vertices[v++] = 0.0f + pointMarkerHeight;
    vertices[v++] = y;
    vertices[v++] = 0.0f;

    vertices[v++] = 0.0f - pointMarkerHeight;
    vertices[v++] = y;
    vertices[v++] = 0.0f;

    indices[idx++] = (GLuint)indicesVal++;
    indices[idx++] = (GLuint)indicesVal++;
  }

  // Update VBO and EBO
  glBindVertexArray(*VAO);

  glBindBuffer(GL_ARRAY_BUFFER, *VBO);
  glBufferData(GL_ARRAY_BUFFER, (long unsigned int)vertexCount * (long unsigned int)3 * sizeof(float), vertices, GL_DYNAMIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, *EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, (long unsigned int)vertexCount * sizeof(GLuint), indices, GL_DYNAMIC_DRAW);

  // Render
  glUseProgram(*program);
  rsu_GluSet4f(*program, "color", 1.0f, 1.0f, 1.0f, 1.0f);
  rsu_GluSetMat4(*program, "graphProjection", *projectionMatrixPtr);
  glLineWidth(2.0f);
  glDrawElements(GL_LINES, vertexCount, GL_UNSIGNED_INT, 0);

  glBindVertexArray(0);

  // Cleanup
  free(vertices);
  free(indices);

  return ERR_SUCCESS;
}
