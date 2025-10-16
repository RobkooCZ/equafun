#ifndef SHADER_UTILS_H
#define SHADER_UTILS_H

#include <GLFW/glfw3.h>

#include "core/errorHandler.h"

enum reh_error_code_e loadShaderSource(const char *path, char **outSource);
enum reh_error_code_e compileShader(const char *source, unsigned int shaderType, GLuint *outShader);
enum reh_error_code_e linkShaders(GLuint vertex, GLuint fragment, GLuint *outProgram);

void gluSetFloat(GLuint program, const char *name, const float value);
void gluSet3f(GLuint program, const char *name, const float x, const float y, const float z);
void gluSet4f(GLuint program, const char *name, const float x, const float y, const float z, const float w);
void gluSetMat4(GLuint program, const char *name, const float *value);
#endif // SHADER_UTILS_H
