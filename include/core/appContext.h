#ifndef APP_CONTEXT_H
#define APP_CONTEXT_H

#include "glad/glad.h"
#include <GLFW/glfw3.h>
#include <ft2build.h>
#include FT_FREETYPE_H


struct ra_app_context_t {
  // window handle - null if failure
  GLFWwindow *window;

  // graph resources - 0 if failure
  GLuint gVAO, gVBO, gEBO, gProgram;

  // marker resources - 0 if failure
  GLuint gmVAO, gmVBO, gmEBO, gmProgram;

  // function resources - 0 if failure
  // EBO not necessary as glDrawVertexArrays() will be used
  GLuint fVAO, fVBO, fProgram;
  GLsizeiptr fVertexCapacityBytes;

  // Freetype - non-zero error code if failure
  FT_Library ft;
  FT_Face face;

  // shaders and stuff
  char* vertexShaderSrc; // has to be freed
  char* fragShaderSrc;   // has to be freed
  GLuint vertexShader, fragShader, textProgram;

  // text rendering resources
  GLuint textVAO, textVBO;
};

void ra_appContextCleanup(struct ra_app_context_t *context, const char* msg);

#endif // APP_CONTEXT_H
