#ifndef SHADER_UTILS_H
#define SHADER_UTILS_H

#include <GLFW/glfw3.h>

#include "core/errorHandler.h"

enum ErrorCode loadShaderSource(const char *path, char **outSource);
enum ErrorCode compileShader(const char *source, unsigned int shaderType, GLuint *outShader);
enum ErrorCode linkShaders(GLuint vertex, GLuint fragment, GLuint *outProgram);

void gluSetFloat(GLuint program, const char *name, const float value);
void gluSet4f(GLuint program, const char *name, const float x, const float y, const float z, const float w);

#endif // SHADER_UTILS_H