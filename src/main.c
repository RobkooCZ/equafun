#include "glad/glad.h"
#include <GLFW/glfw3.h>

#include "core/window.h"
#include "core/input.h"
#include "core/logger.h"
#include "core/errorHandler.h"
#include "renderer/graph.h"

int main(void){
  GLFWwindow *window;

  enum ErrorCode err = initGLFW();
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
  err = setupMarkers(&gmProgram, &gmVAO, &gmVBO, &gmEBO);
  if (err != ERR_SUCCESS){
    logLastError(ERROR);
    logMsg(FAILURE, "Application cannot continue: Graph marker rendering system failed to initialize");
    glfwDestroyWindow(window);
    glfwTerminate();
    return -1;
  }

  while (!glfwWindowShouldClose(window)){
    processInput(window);

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    renderGraph(&gProgram, &gVAO);
    renderMarkers(&gmProgram, &gmVAO);

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  glfwTerminate();
  return 0;
}
