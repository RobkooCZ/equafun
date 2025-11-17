/**
  ra - Robkoo's Application
*/

#ifndef APP_CONTEXT_H
#define APP_CONTEXT_H

#include "glad/glad.h"
#include <GLFW/glfw3.h>
#include <ft2build.h>
#include FT_FREETYPE_H

/**
  @brief Application context structure holding resources and state
*/
struct ra_app_context_t {
  GLFWwindow *window;           /**< Window handle; nullptr on failure. */

  /* Graph resources */
  GLuint gVAO;                  /**< Vertex Array Object for graphs; 0 on failure. */
  GLuint gVBO;                  /**< Vertex Buffer Object for graphs; 0 on failure. */
  GLuint gEBO;                  /**< Element Buffer Object for graphs; 0 on failure. */
  GLuint gProgram;              /**< Shader program for graphs; 0 on failure. */

  /* Marker resources */
  GLuint gmVAO;                 /**< Vertex Array Object for markers; 0 on failure. */
  GLuint gmVBO;                 /**< Vertex Buffer Object for markers; 0 on failure. */
  GLuint gmEBO;                 /**< Element Buffer Object for markers; 0 on failure. */
  GLuint gmProgram;             /**< Shader program for markers; 0 on failure. */

  /* Function resources
     EBO is not necessary as glDrawArrays() will be used. */
  GLuint fVAO;                  /**< Vertex Array Object for functions; 0 on failure. */
  GLuint fVBO;                  /**< Vertex Buffer Object for functions; 0 on failure. */
  GLuint fProgram;              /**< Shader program for functions; 0 on failure. */
  GLsizeiptr fVertexCapacityBytes; /**< Allocated capacity for function vertex buffer in bytes. */

  /* FreeType */
  FT_Library ft;                /**< FreeType library handle. */
  FT_Face face;                 /**< FreeType face object. */

  /* Shaders and programs */
  char* vertexShaderSrc;        /**< Owned vertex shader source; must be freed. */
  char* fragShaderSrc;          /**< Owned fragment shader source; must be freed. */
  GLuint vertexShader;          /**< Compiled vertex shader object; 0 on failure. */
  GLuint fragShader;            /**< Compiled fragment shader object; 0 on failure. */
  GLuint textProgram;           /**< Shader program used for text rendering; 0 on failure. */

  /* Text rendering resources */
  GLuint textVAO;               /**< Vertex Array Object for text rendering; 0 on failure. */
  GLuint textVBO;               /**< Vertex Buffer Object for text rendering; 0 on failure. */
};

/**
  @brief Cleans up the application context and releases resources
*/
void ra_AppContextCleanup(struct ra_app_context_t *context, const char* msg);

#endif // APP_CONTEXT_H
