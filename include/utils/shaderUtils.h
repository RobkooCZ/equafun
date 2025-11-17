/**
  rsu - Robkoo's Shader Utilities
*/

#ifndef SHADER_UTILS_H
#define SHADER_UTILS_H

#include <GLFW/glfw3.h>

#include "core/errorHandler.h"

/**
  @brief Loads shader source code from a file
*/
enum reh_error_code_e rsu_LoadShaderSource(const char *path, char **outSource);

/**
  @brief Compiles a shader from source code
*/
enum reh_error_code_e rsu_CompileShader(const char *source, unsigned int shaderType, GLuint *outShader);

/**
  @brief Links vertex and fragment shaders into a shader program
*/
enum reh_error_code_e rsu_LinkShaders(GLuint vertex, GLuint fragment, GLuint *outProgram);

/**
  @brief Sets a float uniform in the shader program
*/
void rsu_GluSetFloat(GLuint program, const char *name, const float value);

/**
  @brief Sets a vec3 uniform in the shader program
*/
void rsu_GluSet3f(GLuint program, const char *name, const float x, const float y, const float z);

/**
  @brief Sets a vec4 uniform in the shader program
*/
void rsu_GluSet4f(GLuint program, const char *name, const float x, const float y, const float z, const float w);

/**
  @brief Sets a mat4 uniform in the shader program
*/
void rsu_GluSetMat4(GLuint program, const char *name, const float *value);
#endif // SHADER_UTILS_H
