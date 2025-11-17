#include "glad/glad.h"
#include <GLFW/glfw3.h>

#include "core/logger.h"
#include "utils/shaderUtils.h"
#include "core/errorHandler.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

enum reh_error_code_e rsu_LoadShaderSource(const char *path, char **outSource){
  if (!path || !outSource){
    SET_ERROR_RETURN(ERR_INVALID_POINTER, "Invalid parameters: path or outSource is NULL");
  }

  FILE* shaderSrcFile = fopen(path, "rb");
  rl_LogMsg(RL_DEBUG, "Attempting to open file at path: %s", path);

  if (!shaderSrcFile){
    char technical[512];
    snprintf(technical, sizeof(technical), "fopen() failed with errno %d: %s", errno, strerror(errno));
    SET_ERROR_TECHNICAL_RETURN(ERR_FILE_NOT_FOUND, "Failed to open shader file: %s", technical, path);
  }

  rl_LogMsg(RL_DEBUG, "Successfully opened file at path: %s", path);

  fseek(shaderSrcFile, 0L, SEEK_END);
  long fSize = ftell(shaderSrcFile);
  fseek(shaderSrcFile, 0L, SEEK_SET);

  if (fSize < 0){
    fclose(shaderSrcFile);
    char technical[512];
    snprintf(technical, sizeof(technical), "ftell() returned %ld", fSize);
    SET_ERROR_TECHNICAL_RETURN(ERR_FILE_READ_FAILED, "Failed to determine file size for: %s", technical, path);
  }

  rl_LogMsg(RL_DEBUG, "Size of read file contents: %ld bytes.", fSize);

  char* shaderSrc = (char*)malloc((long unsigned int)(fSize + 1) * sizeof(char));
  if (!shaderSrc){
    fclose(shaderSrcFile);
    SET_ERROR_RETURN(ERR_OUT_OF_MEMORY, "Failed to allocate %ld bytes for shader source", fSize + 1);
  }

  // reads 161 bytes (win)
  size_t bytesRead = fread(shaderSrc, 1, (long unsigned int)fSize, shaderSrcFile);
  fclose(shaderSrcFile);

  if (bytesRead != (size_t)fSize){
    free(shaderSrc);
    char technical[512];
    snprintf(technical, sizeof(technical), "fread() returned %zu bytes, expected %ld", bytesRead, fSize);
    SET_ERROR_TECHNICAL_RETURN(ERR_FILE_READ_FAILED, "Failed to read complete file: %s", technical, path);
  }

  shaderSrc[fSize] = '\0';
  rl_LogMsg(RL_DEBUG, "Read file's contents:\n%s", shaderSrc);

  *outSource = shaderSrc;
  return ERR_SUCCESS;
}

enum reh_error_code_e rsu_CompileShader(const char *source, unsigned int shaderType, GLuint *outShader){
  if (!source){
    SET_ERROR_RETURN(ERR_SHADER_SOURCE_NULL, "Shader source is NULL");
  }

  if (!outShader){
    SET_ERROR_RETURN(ERR_INVALID_POINTER, "Output shader pointer is NULL");
  }

  if (shaderType != GL_VERTEX_SHADER && shaderType != GL_FRAGMENT_SHADER){
    SET_ERROR_RETURN(ERR_SHADER_INVALID_TYPE, "Invalid shader type: %u (expected GL_VERTEX_SHADER or GL_FRAGMENT_SHADER)", shaderType);
  }

  GLuint shader = glCreateShader(shaderType);
  glShaderSource(shader, 1, &source, nullptr);
  glCompileShader(shader);

  int success;
  glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

  if (!success){
    char infoLog[512];
    glGetShaderInfoLog(shader, 512, nullptr, infoLog);
    glDeleteShader(shader);
    
    const char* shaderTypeStr = (shaderType == GL_VERTEX_SHADER) ? "GL_VERTEX_SHADER" : "GL_FRAGMENT_SHADER";
    SET_ERROR_TECHNICAL_RETURN(ERR_SHADER_COMPILE_FAILED, "Shader compilation failed for %s", infoLog, shaderTypeStr);
  }

  rl_LogMsg(RL_DEBUG, "Successfully created and compiled shader with type %s.", (shaderType == GL_VERTEX_SHADER) ? "GL_VERTEX_SHADER" : "GL_FRAGMENT_SHADER");

  *outShader = shader;
  return ERR_SUCCESS;
}

enum reh_error_code_e rsu_LinkShaders(GLuint vertex, GLuint fragment, GLuint* outProgram){
  if (!outProgram){
    SET_ERROR_RETURN(ERR_INVALID_POINTER, "Output program pointer is NULL");
  }

  if (vertex == 0){
    SET_ERROR_RETURN(ERR_SHADER_COMPILE_FAILED, "Invalid vertex shader ID (0)");
  }

  if (fragment == 0){
    SET_ERROR_RETURN(ERR_SHADER_COMPILE_FAILED, "Invalid fragment shader ID (0)");
  }

  GLuint program = glCreateProgram();
  glAttachShader(program, vertex);
  glAttachShader(program, fragment);
  glLinkProgram(program);

  int success;
  glGetProgramiv(program, GL_LINK_STATUS, &success);

  if (!success){
    char infoLog[512];
    glGetProgramInfoLog(program, 512, nullptr, infoLog);
    glDeleteProgram(program);
    
    SET_ERROR_TECHNICAL_RETURN(ERR_SHADER_LINK_FAILED, "Failed to link shader program", infoLog);
  }

  rl_LogMsg(RL_DEBUG, "Successfully linked shaders.");

  glDeleteShader(vertex);
  glDeleteShader(fragment);

  *outProgram = program;
  return ERR_SUCCESS;
}

void rsu_GluSetFloat(GLuint program, const char *name, const float value){
  if (name == nullptr){
    rl_LogMsg(RL_WARNING, "Uniform name is NULL in rsu_GluSetFloat()");
    return;
  }

  GLint location = glGetUniformLocation(program, name);
  if (location == -1){
    rl_LogMsg(RL_WARNING, "Failed to find uniform '%s' in program %u", name, program);
    return;
  }

  glUniform1f(location, value);
}

void rsu_GluSet3f(GLuint program, const char *name, const float x, const float y, const float z){
  if (name == nullptr){
    rl_LogMsg(RL_WARNING, "Uniform name is NULL in rsu_GluSet3f()");
    return;
  }

  GLint location = glGetUniformLocation(program, name);
  if (location == -1){
    rl_LogMsg(RL_WARNING, "Failed to find uniform '%s' in program %u", name, program);
    return;
  }

  glUniform3f(location, x, y, z);
}

void rsu_GluSet4f(GLuint program, const char *name, const float x, const float y, const float z, const float w){
  if (name == nullptr){
    rl_LogMsg(RL_WARNING, "Uniform name is NULL in rsu_GluSet4f()");
    return;
  }

  GLint location = glGetUniformLocation(program, name);
  if (location == -1){
    rl_LogMsg(RL_WARNING, "Failed to find uniform '%s' in program %u", name, program);
    return;
  }

  glUniform4f(location, x, y, z, w);
}

void rsu_GluSetMat4(GLuint program, const char *name, const float *value){
  if (name == nullptr){
    rl_LogMsg(RL_WARNING, "Uniform name is NULL in rsu_GluSetMat4()");
    return;
  }

  if (value == nullptr){
    rl_LogMsg(RL_WARNING, "Matrix value is NULL in rsu_GluSetMat4()");
    return;
  }

  GLint location = glGetUniformLocation(program, name);
  if (location == -1){
    rl_LogMsg(RL_WARNING, "Uniform '%s' not found in shader program", name);
    return;
  }

  glUniformMatrix4fv(location, 1, GL_FALSE, value);
}
