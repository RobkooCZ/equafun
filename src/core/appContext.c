#include "core/appContext.h"
#include "core/logger.h"

#include "freetype/freetype.h"
#include "glad/glad.h"
#include <GLFW/glfw3.h>
#include <stdlib.h>
#include <string.h>

void ra_appContextCleanup(struct ra_app_context_t *context, const char *msg){
  if (context == nullptr){
    logMsg(FAILURE, "NULL context passed to ra_appContextCleanup.");
    return;
  }

  // log errors
  logLastError(ERROR);
  logMsg(FAILURE, msg);

  // clean up resources conditionally
  if (context->window != nullptr){
    glfwDestroyWindow(context->window);
  }
  if (context->gVAO != 0){
    glDeleteVertexArrays(1, &context->gVAO);
  }
  if (context->gVBO != 0){
    glDeleteBuffers(1, &context->gVBO);
  }
  if (context->gEBO != 0){
    glDeleteBuffers(1, &context->gEBO);
  }
  if (context->gProgram != 0){
    glDeleteProgram(context->gProgram);
  }
  if (context->gmVAO != 0){
    glDeleteVertexArrays(1, &context->gmVAO);
  }
  if (context->gmVBO != 0){
    glDeleteBuffers(1, &context->gmVBO);
  }
  if (context->gmEBO != 0){
    glDeleteBuffers(1, &context->gmEBO);
  }
  if (context->gmProgram != 0){
    glDeleteProgram(context->gmProgram);
  }
  if (context->fVAO != 0){
    glDeleteVertexArrays(1, &context->fVAO);
  }
  if (context->fVBO != 0){
    glDeleteBuffers(1, &context->fVBO);
  }
  if (context->fProgram != 0){
    glDeleteProgram(context->fProgram);
  }
  if (context->face != nullptr){
    FT_Done_Face(context->face);
  }
  if (context->ft != nullptr){
    FT_Done_FreeType(context->ft);
  }
  if (context->vertexShaderSrc != nullptr){
    free(context->vertexShaderSrc);
  }
  if (context->fragShaderSrc != nullptr){
    free(context->fragShaderSrc);
  }
  if (context->vertexShader != 0){
    glDeleteShader(context->vertexShader);
  }
  if (context->fragShader != 0){
    glDeleteShader(context->fragShader);
  }
  if (context->textVAO != 0){
    glDeleteVertexArrays(1, &context->textVAO);
  }
  if (context->textVBO != 0){
    glDeleteBuffers(1, &context->textVBO);
  }

  // clear struct fields
  memset(context, 0, sizeof *context);

  // always there
  glfwTerminate();
}
