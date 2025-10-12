#ifndef RENDER_UTILS_H
#define RENDER_UTILS_H

#include <GLFW/glfw3.h>

#include "core/errorHandler.h"

enum ErrorCode setupRenderData(float *vertices, size_t verticesSize, GLuint *indices, size_t indicesSize, GLuint *VAO, GLuint *VBO, GLuint *EBO);
enum ErrorCode setupEBO(GLuint *indices, size_t indicesSize, GLuint *EBO);

#endif // RENDER_UTILS_H