#ifndef SHADER_UTILS_H
#define SHADER_UTILS_H

#include <GLFW/glfw3.h>
#include <stdio.h>

FILE *openFile(const char* path);
long getFileSize(FILE *file);
char* readFileToString(FILE *file, const long fSize);

// implements all the above functions
char* loadShaderSource(const char* path);

GLuint compileShader(const char* source, unsigned int shaderType);
GLuint linkShaders(GLuint vertex, GLuint fragment);

void gluSetFloat(GLuint program, const char* name, const float value);
void gluSet4f(GLuint program, const char *name, const float x, const float y, const float z, const float w);

#endif // SHADER_UTILS_H
