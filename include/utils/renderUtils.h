/**
  rru - Robkoo's Render Utilities
*/

#ifndef RENDER_UTILS_H
#define RENDER_UTILS_H

#include <GLFW/glfw3.h>

#include "core/errorHandler.h"

/**
  @brief Sets up the VAO, VBO, and EBO for rendering
*/
enum reh_error_code_e rru_SetupRenderData(float *vertices, size_t verticesSize, GLuint *indices, size_t indicesSize, GLuint *VAO, GLuint *VBO, GLuint *EBO);

/**
  @brief Sets up the VBO for rendering
*/
enum reh_error_code_e rru_SetupEbo(GLuint *indices, size_t indicesSize, GLuint *EBO);

#endif // RENDER_UTILS_H