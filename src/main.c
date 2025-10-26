#include "expressionEngine/lexer.h"
#include "glad/glad.h"
#include <GLFW/glfw3.h>

#include "core/logger.h"
#include "core/errorHandler.h"

// window
#include "core/window.h"
#include "core/input.h"

// graph renderer
#include "renderer/graph.h"

// text renderer
#include "textRenderer/text.h"
#include "utils/shaderUtils.h"
#include "math/Mat4.h"
#include "math/Vec3.h"

// free type
#include <ft2build.h>
#include FT_FREETYPE_H

int main(void){
  // lexer testing 
  char* expression = "  345 + 123-2   =12";

  printf("Expression input: %s\n", expression);
  // ONLY FOR NOW i can pass NULL, there are no checks and NO assignements to the tokens array
  // inside the ree_lexer function.
  ree_lexer(expression, NULL);

  // do not do anything else
  return 0;

  GLFWwindow *window;

  enum reh_error_code_e err = initGLFW();
  if (err != ERR_SUCCESS){
    logLastError(ERROR);
    logMsg(FAILURE, "Application cannot continue: GLFW initialization failed");
    return -1;
  }
  logMsg(SUCCESS, "GLFW initialized successfully");

  err = initWindow(&window);
  if (err != ERR_SUCCESS){
    logLastError(ERROR);
    logMsg(FAILURE, "Application cannot continue: Window creation failed");
    glfwTerminate();
    return -1;
  }
  logMsg(SUCCESS, "Window initialized successfully");

  const GLubyte* version = glGetString(GL_VERSION);
  logMsg(DEBUG, "OpenGL version: %s", version);

  glEnable(GL_LINE_SMOOTH);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glfwSwapInterval(1);

  GLuint gVAO, gVBO, gEBO, gProgram;
  err = setupGraph(&gProgram, &gVAO, &gVBO, &gEBO);
  if (err != ERR_SUCCESS){
    logLastError(ERROR);
    logMsg(FAILURE, "Application cannot continue: Graph axis rendering system failed to initialize");
    glfwDestroyWindow(window);
    glfwTerminate();
    return -1;
  }

  GLuint gmVAO, gmVBO, gmEBO, gmProgram;
  err = setupMarkerShaders(&gmProgram);
  if (err != ERR_SUCCESS){
    logLastError(ERROR);
    logMsg(FAILURE, "Application cannot continue: Graph marker shaders failed to initialize");
    glfwDestroyWindow(window);
    glfwTerminate();
    return -1;
  }

  err = setupMarkerBuffers(&gmVAO, &gmVBO, &gmEBO);
  if (err != ERR_SUCCESS){
    logLastError(ERROR);
    logMsg(FAILURE, "Application cannot continue: Graph marker buffers failed to initialize");
    glfwDestroyWindow(window);
    glfwTerminate();
    return -1;
  }

  // Initialize FreeType
  FT_Library ft;
  err = rtr_initFt(&ft);
  if (err != ERR_SUCCESS){
    logLastError(ERROR);
    logMsg(FAILURE, "Application cannot continue: FreeType initialization failed");
    glfwDestroyWindow(window);
    glfwTerminate();
    return -1;
  }
  logMsg(SUCCESS, "FreeType initialized successfully");

  // Load font face
  FT_Face face;
  err = rtr_initFtFace(&ft, &face);
  if (err != ERR_SUCCESS){
    logLastError(ERROR);
    logMsg(FAILURE, "Application cannot continue: Font face initialization failed");
    FT_Done_FreeType(ft);
    glfwDestroyWindow(window);
    glfwTerminate();
    return -1;
  }
  logMsg(SUCCESS, "Font face initialized successfully");

  // Load characters
  struct rtr_character_t characters[ASCII_CHAR_COUNT];
  err = rtr_loadCharactersIntoArray(face, characters);
  if (err != ERR_SUCCESS){
    logLastError(ERROR);
    logMsg(FAILURE, "Application cannot continue: Failed to load characters");
    FT_Done_Face(face);
    FT_Done_FreeType(ft);
    glfwDestroyWindow(window);
    glfwTerminate();
    return -1;
  }
  logMsg(SUCCESS, "Characters loaded successfully");

  // Load text rendering shader
  char *vertexShaderSrc = nullptr;
  char *fragmentShaderSrc = nullptr;

  err = loadShaderSource("data/shaders/textRender.vert", &vertexShaderSrc);
  if (err != ERR_SUCCESS){
    logLastError(ERROR);
    logMsg(FAILURE, "Application cannot continue: Failed to load text vertex shader");
    glfwDestroyWindow(window);
    glfwTerminate();
    return -1;
  }

  err = loadShaderSource("data/shaders/textColor.frag", &fragmentShaderSrc);
  if (err != ERR_SUCCESS){
    free(vertexShaderSrc);
    logLastError(ERROR);
    logMsg(FAILURE, "Application cannot continue: Failed to load text fragment shader");
    glfwDestroyWindow(window);
    glfwTerminate();
    return -1;
  }

  GLuint vertexShader = 0;
  GLuint fragShader = 0;

  err = compileShader(vertexShaderSrc, GL_VERTEX_SHADER, &vertexShader);
  if (err != ERR_SUCCESS){
    free(vertexShaderSrc);
    free(fragmentShaderSrc);
    logLastError(ERROR);
    logMsg(FAILURE, "Application cannot continue: Failed to compile text vertex shader");
    glfwDestroyWindow(window);
    glfwTerminate();
    return -1;
  }

  err = compileShader(fragmentShaderSrc, GL_FRAGMENT_SHADER, &fragShader);
  if (err != ERR_SUCCESS){
    free(vertexShaderSrc);
    free(fragmentShaderSrc);
    glDeleteShader(vertexShader);
    logLastError(ERROR);
    logMsg(FAILURE, "Application cannot continue: Failed to compile text fragment shader");
    glfwDestroyWindow(window);
    glfwTerminate();
    return -1;
  }

  GLuint textProgram;
  err = linkShaders(vertexShader, fragShader, &textProgram);
  free(vertexShaderSrc);
  free(fragmentShaderSrc);

  if (err != ERR_SUCCESS){
    logLastError(ERROR);
    logMsg(FAILURE, "Application cannot continue: Failed to link text shaders");
    glfwDestroyWindow(window);
    glfwTerminate();
    return -1;
  }
  logMsg(SUCCESS, "Text rendering shader program created successfully");

  // Create text rendering VAO/VBO
  GLuint textVAO, textVBO;
  err = rtr_createTextRenderVAO(&textVAO, &textVBO);
  if (err != ERR_SUCCESS){
    logLastError(ERROR);
    logMsg(FAILURE, "Application cannot continue: Failed to create text rendering VAO/VBO");
    glDeleteProgram(textProgram);
    glfwDestroyWindow(window);
    glfwTerminate();
    return -1;
  }

  // Setup orthographic projection for text rendering
  struct rm_mat4_t projection;
  err = rm_Mat4Ortho(0.0f, WIDTH, 0.0f, HEIGHT, &projection);
  if (err != ERR_SUCCESS){
    logLastError(ERROR);
    logMsg(FAILURE, "Application cannot continue: Failed to create projection matrix");
    glDeleteProgram(textProgram);
    glfwDestroyWindow(window);
    glfwTerminate();
    return -1;
  }

  float *projectionPtr = nullptr;
  err = rm_Mat4ValuePtr(&projection, &projectionPtr);
  if (err != ERR_SUCCESS){
    logLastError(ERROR);
    logMsg(FAILURE, "Application cannot continue: Failed to get projection matrix pointer");
    glDeleteProgram(textProgram);
    glfwDestroyWindow(window);
    glfwTerminate();
    return -1;
  }

  glUseProgram(textProgram);
  gluSetMat4(textProgram, "projection", projectionPtr);

  while (!glfwWindowShouldClose(window)){
    processInput(window);

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    err = renderGraph(&gProgram, &gVAO);

    if (err != ERR_SUCCESS){
      logLastError(ERROR);
      logMsg(FAILURE, "Application cannot continue: Failed to render graph.");
      glDeleteProgram(textProgram);
      glfwDestroyWindow(window);
      glfwTerminate();
      return -1;
    }

    err = renderMarkers(&gmProgram, &gmVAO, &gmVBO, &gmEBO);

    if (err != ERR_SUCCESS){
      logLastError(ERROR);
      logMsg(FAILURE, "Application cannot continue: Failed to render graph markers.");
      glDeleteProgram(textProgram);
      glfwDestroyWindow(window);
      glfwTerminate();
      return -1;
    }
    // Render text
    struct rm_vec3_t textColor = {1.0f, 1.0f, 1.0f};
    err = rtr_renderAxisLabels(textProgram, textVAO, textVBO, characters, 1.0f, textColor);

    if (err != ERR_SUCCESS){
      logLastError(ERROR);
      logMsg(FAILURE, "Application cannot continue: Failed to render graph markers' labels.");
      glDeleteProgram(textProgram);
      glfwDestroyWindow(window);
      glfwTerminate();
      return -1;
    }

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  // Cleanup
  glDeleteVertexArrays(1, &textVAO);
  glDeleteBuffers(1, &textVBO);
  glDeleteProgram(textProgram);
  FT_Done_Face(face);
  FT_Done_FreeType(ft);

  glfwTerminate();
  return 0;
}
