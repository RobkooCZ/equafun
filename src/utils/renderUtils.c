#include "glad/glad.h"
#include <GLFW/glfw3.h>

#include "utils/renderUtils.h"
#include "core/errorHandler.h"
#include "core/logger.h"

#include <stdio.h>

enum reh_error_code_e setupEBO(GLuint *indices, size_t indicesSize, GLuint *EBO){
  if (!indices){
    SET_ERROR_RETURN(ERR_INVALID_POINTER, "Indices pointer is NULL in setupEBO()");
  }

  if (!EBO){
    SET_ERROR_RETURN(ERR_INVALID_POINTER, "EBO pointer is NULL in setupEBO()");
  }

  if (indicesSize == 0){
    SET_ERROR_RETURN(ERR_BUFFER_SETUP_FAILED, "Indices size is 0 in setupEBO()");
  }

  // Clear any previous OpenGL errors
  while (glGetError() != GL_NO_ERROR);

  glGenBuffers(1, EBO);
  GLenum err = glGetError();
  if (err != GL_NO_ERROR){
    char technical[256];
    snprintf(technical, sizeof(technical), "glGenBuffers failed with error: 0x%04X", err);
    SET_ERROR_TECHNICAL_RETURN(ERR_BUFFER_SETUP_FAILED, "Failed to generate Element Buffer Object", technical);
  }

  if (*EBO == 0){
    SET_ERROR_RETURN(ERR_INVALID_EBO, "Generated EBO is 0 (invalid)");
  }

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, *EBO);
  err = glGetError();
  if (err != GL_NO_ERROR){
    char technical[256];
    snprintf(technical, sizeof(technical), "glBindBuffer(GL_ELEMENT_ARRAY_BUFFER) failed with error: 0x%04X", err);
    glDeleteBuffers(1, EBO);
    *EBO = 0;
    SET_ERROR_TECHNICAL_RETURN(ERR_BUFFER_SETUP_FAILED, "Failed to bind Element Buffer Object", technical);
  }

  glBufferData(GL_ELEMENT_ARRAY_BUFFER, indicesSize, indices, GL_STATIC_DRAW);
  err = glGetError();
  if (err != GL_NO_ERROR){
    char technical[256];
    snprintf(technical, sizeof(technical), 
             "glBufferData(GL_ELEMENT_ARRAY_BUFFER, %zu bytes) failed with error: 0x%04X", 
             indicesSize, err);
    glDeleteBuffers(1, EBO);
    *EBO = 0;
    SET_ERROR_TECHNICAL_RETURN(ERR_BUFFER_SETUP_FAILED, "Failed to upload index data to GPU", technical);
  }

  logMsg(DEBUG, "Successfully set up EBO with %zu bytes of index data", indicesSize);
  return ERR_SUCCESS;
}

enum reh_error_code_e setupRenderData(float *vertices, size_t verticesSize, GLuint *indices, size_t indicesSize, GLuint *VAO, GLuint *VBO, GLuint *EBO){
  if (!vertices){
    SET_ERROR_RETURN(ERR_INVALID_POINTER, "Vertices pointer is NULL in setupRenderData()");
  }

  if (!indices){
    SET_ERROR_RETURN(ERR_INVALID_POINTER, "Indices pointer is NULL in setupRenderData()");
  }

  if (!VAO){
    SET_ERROR_RETURN(ERR_INVALID_POINTER, "VAO pointer is NULL in setupRenderData()");
  }

  if (!VBO){
    SET_ERROR_RETURN(ERR_INVALID_POINTER, "VBO pointer is NULL in setupRenderData()");
  }

  if (!EBO){
    SET_ERROR_RETURN(ERR_INVALID_POINTER, "EBO pointer is NULL in setupRenderData()");
  }

  if (verticesSize == 0){
    SET_ERROR_RETURN(ERR_BUFFER_SETUP_FAILED, "Vertices size is 0 in setupRenderData()");
  }

  if (indicesSize == 0){
    SET_ERROR_RETURN(ERR_BUFFER_SETUP_FAILED, "Indices size is 0 in setupRenderData()");
  }

  // Clear any previous OpenGL errors
  while (glGetError() != GL_NO_ERROR);

  // Generate VAO
  glGenVertexArrays(1, VAO);
  GLenum err = glGetError();
  if (err != GL_NO_ERROR){
    char technical[256];
    snprintf(technical, sizeof(technical), "glGenVertexArrays failed with error: 0x%04X", err);
    SET_ERROR_TECHNICAL_RETURN(ERR_BUFFER_SETUP_FAILED, "Failed to generate Vertex Array Object", technical);
  }

  if (*VAO == 0){
    SET_ERROR_RETURN(ERR_INVALID_VAO, "Generated VAO is 0 (invalid)");
  }

  // Generate VBO
  glGenBuffers(1, VBO);
  err = glGetError();
  if (err != GL_NO_ERROR){
    char technical[256];
    snprintf(technical, sizeof(technical), "glGenBuffers(VBO) failed with error: 0x%04X", err);
    glDeleteVertexArrays(1, VAO);
    *VAO = 0;
    SET_ERROR_TECHNICAL_RETURN(ERR_BUFFER_SETUP_FAILED, "Failed to generate Vertex Buffer Object", technical);
  }

  if (*VBO == 0){
    glDeleteVertexArrays(1, VAO);
    *VAO = 0;
    SET_ERROR_RETURN(ERR_INVALID_VBO, "Generated VBO is 0 (invalid)");
  }

  // Bind VAO
  glBindVertexArray(*VAO);
  err = glGetError();
  if (err != GL_NO_ERROR){
    char technical[256];
    snprintf(technical, sizeof(technical), "glBindVertexArray failed with error: 0x%04X", err);
    glDeleteBuffers(1, VBO);
    glDeleteVertexArrays(1, VAO);
    *VAO = 0;
    *VBO = 0;
    SET_ERROR_TECHNICAL_RETURN(ERR_BUFFER_SETUP_FAILED, "Failed to bind Vertex Array Object", technical);
  }

  // Bind and upload VBO data
  glBindBuffer(GL_ARRAY_BUFFER, *VBO);
  err = glGetError();
  if (err != GL_NO_ERROR){
    char technical[256];
    snprintf(technical, sizeof(technical), "glBindBuffer(GL_ARRAY_BUFFER) failed with error: 0x%04X", err);
    glDeleteBuffers(1, VBO);
    glDeleteVertexArrays(1, VAO);
    *VAO = 0;
    *VBO = 0;
    SET_ERROR_TECHNICAL_RETURN(ERR_BUFFER_SETUP_FAILED, "Failed to bind Vertex Buffer Object", technical);
  }

  glBufferData(GL_ARRAY_BUFFER, verticesSize, vertices, GL_STATIC_DRAW);
  err = glGetError();
  if (err != GL_NO_ERROR){
    char technical[256];
    snprintf(technical, sizeof(technical), "glBufferData(GL_ARRAY_BUFFER, %zu bytes) failed with error: 0x%04X", verticesSize, err);
    glDeleteBuffers(1, VBO);
    glDeleteVertexArrays(1, VAO);
    *VAO = 0;
    *VBO = 0;
    SET_ERROR_TECHNICAL_RETURN(ERR_BUFFER_SETUP_FAILED, "Failed to upload vertex data to GPU", technical);
  }

  // Set vertex attribute pointer
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
  err = glGetError();
  if (err != GL_NO_ERROR){
    char technical[256];
    snprintf(technical, sizeof(technical), "glVertexAttribPointer failed with error: 0x%04X", err);
    glDeleteBuffers(1, VBO);
    glDeleteVertexArrays(1, VAO);
    *VAO = 0;
    *VBO = 0;
    SET_ERROR_TECHNICAL_RETURN(ERR_BUFFER_SETUP_FAILED, "Failed to set vertex attribute pointer", technical);
  }

  glEnableVertexAttribArray(0);
  err = glGetError();
  if (err != GL_NO_ERROR){
    char technical[256];
    snprintf(technical, sizeof(technical), "glEnableVertexAttribArray failed with error: 0x%04X", err);
    glDeleteBuffers(1, VBO);
    glDeleteVertexArrays(1, VAO);
    *VAO = 0;
    *VBO = 0;
    SET_ERROR_TECHNICAL_RETURN(ERR_BUFFER_SETUP_FAILED, "Failed to enable vertex attribute array", technical);
  }

  // Setup EBO (BEFORE unbinding VBO)
  enum reh_error_code_e eboErr = setupEBO(indices, indicesSize, EBO);
  if (eboErr != ERR_SUCCESS){
    glDeleteBuffers(1, VBO);
    glDeleteVertexArrays(1, VAO);
    *VAO = 0;
    *VBO = 0;
    SET_ERROR_RETURN(eboErr, "Failed to setup Element Buffer Object in setupRenderData()");
  }

  // Unbind buffers and VAO
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);

  logMsg(DEBUG, "Successfully set up render data (VAO: %u, VBO: %u, EBO: %u, vertices: %zu bytes, indices: %zu bytes)", *VAO, *VBO, *EBO, verticesSize, indicesSize);

  return ERR_SUCCESS;
}