#include "core/logger.h"

#include "glad/glad.h"
#include <GLFW/glfw3.h>

#include "utils/shaderUtils.h"

#include <stdio.h>
#include <stdlib.h>

FILE *openFile(const char* path){
  FILE *file = fopen(path, "r");
  logMsg(DEBUG, "Opened file at path: %s", path);

  if (file == nullptr){
    logMsg(ERROR, "Failed to open file at path: %s", path);
    return nullptr;
  }

  logMsg(DEBUG, "Successfully opened file at path: %s", path);

  return file;
}

long getFileSize(FILE *file){
  if (file == nullptr){
    logMsg(ERROR, "File provided to function 'getFileSize()' is NULL.");
    return -1;
  }

  fseek(file, 0L, SEEK_END);
  long size = ftell(file);
  logMsg(DEBUG, "Size of read file contents: %ld bytes.", size);

  return size;
}

char* readFileToString(FILE *file, const long fSize){
  if (file == nullptr){
    logMsg(ERROR, "File provided to 'readFileToString()' is NULL.");
    return nullptr;
  }
  // fSize + 1 due to the fact we need to append '\0' after the file contents as OpenGL wants null terminated strings
  char* shaderSrc = (char *)malloc((fSize + 1) * sizeof(char));

  if (shaderSrc == nullptr){
    logMsg(ERROR, "Pointer to shaderSrc in 'readFileToString' failed to initialize.");
    return nullptr;
  }

  // read the file contents into a string
  fseek(file, 0L, SEEK_SET);
  fread(shaderSrc, 1, fSize, file);
  shaderSrc[fSize] = '\0'; // null terminate the string

  logMsg(DEBUG, "Read file's contents:\n%s", shaderSrc);

  return shaderSrc;
}

char* loadShaderSource(const char* path){
  FILE *shaderSrcFile = openFile(path);

  // failed to open file
  if (shaderSrcFile == nullptr){
    fclose(shaderSrcFile);
    return nullptr;
  }

  long fSize = getFileSize(shaderSrcFile);

  // failed to get size
  if (fSize == -1){
    fclose(shaderSrcFile);
    return nullptr;
  }

  char* shaderSrc = readFileToString(shaderSrcFile, fSize);

  // failed to read file into the string
  if (shaderSrc == nullptr){
    fclose(shaderSrcFile);
    return nullptr;
  }

  fclose(shaderSrcFile);
  return shaderSrc;
}

GLuint compileShader(const char *source, unsigned int shaderType){
  // arg checks
  if (source == nullptr){
    logMsg(ERROR, "Shader source provided in 'compileShader()' is NULL.");
    return 0;
  }

  if (shaderType != GL_VERTEX_SHADER && shaderType != GL_FRAGMENT_SHADER){
    logMsg(ERROR, "Shader type provided in 'compileShader()' is not valid: %d", shaderType);
    return 0;
  }

  GLuint shader = glCreateShader(shaderType);

  glShaderSource(shader, 1, &source, NULL);
  glCompileShader(shader);

  // check for errors
  int success;
  glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

  if (success == false){
    char infoLog[512];
    glGetShaderInfoLog(shader, 512, NULL, infoLog);
    logMsg(ERROR, "Failed to compile shader:\n%s", infoLog);
    return 0;
  }
  else {
    logMsg(DEBUG, "Successfully created and compiled shader with type %s.", (shaderType == GL_VERTEX_SHADER)? "GL_VERTEX_SHADER" : "GL_FRAGMENT_SHADER");
  }

  // id of the shader
  return shader;
}

GLuint linkShaders(GLuint vertex, GLuint fragment){
  GLuint program = glCreateProgram();

  glAttachShader(program, vertex);
  glAttachShader(program, fragment);
  glLinkProgram(program);

  int success;
  glGetProgramiv(program, GL_LINK_STATUS, &success);

  if (success == false){
    char infoLog[512];
    glGetProgramInfoLog(program, 512, NULL, infoLog);
    logMsg(ERROR, "Failed to link shaders:\n%s", infoLog);
    return 0;
  }
  else {
    logMsg(DEBUG, "Successfully linked shaders.");
  }

  glDeleteShader(vertex);
  glDeleteShader(fragment);

  return program;
}

void gluSetFloat(GLuint program, const char *name, const float value){
  if (name == nullptr){
    logMsg(ERROR, "Name provided in 'gluSetFloat()' is NULL.");
    return;
  }

  glUniform1f(glGetUniformLocation(program, name), value);
}

void gluSet4f(GLuint program, const char *name, const float x, const float y, const float z, const float w){
  if (name == nullptr){
    logMsg(ERROR, "Name provided in 'gluSet4f()' is NULL.");
    return;
  }

  GLint location = glGetUniformLocation(program, name);
  if (location == -1){
    logMsg(ERROR, "Failed to get uniform %s location.", name);
    return;
  }
  else{
    glUniform4f(location, x, y, z, w);
  }
}
