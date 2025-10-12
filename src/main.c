#include "core/logger.h"

#include "glad/glad.h"
#include <GLFW/glfw3.h>

#include "core/window.h"
#include "core/input.h"
#include "renderer/graph.h"

int main(void){
  GLFWwindow *window;

  // init glfw and window and make sure it went well
  if (initGLFW() != 0) return -1;
  else logMsg(SUCCESS, "GLFW initialized successfully");

  if (initWindow(&window) != 0) return -1;
  else logMsg(SUCCESS, "Window initialized successfully");

  const GLubyte* version = glGetString(GL_VERSION);
  logMsg(DEBUG, "OpenGL version: %s", version);

  glEnable(GL_LINE_SMOOTH);

  // VAO, VBO and EBO for graph
  GLuint gVAO, gVBO, gEBO, gProgram;
  setupGraph(&gProgram, &gVAO, &gVBO, &gEBO);

  // VAO, VBO and EBO for graph markers
  GLuint gmVAO, gmVBO, gmEBO, gmProgram;
  setupMarkers(&gmProgram, &gmVAO, &gmVBO, &gmEBO);

  // render loop
  while (!glfwWindowShouldClose(window)){
    // process input
    processInput(window);

    // background color
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // graph render
    renderGraph(&gProgram, &gVAO);

    // graph markers render
    renderMarkers(&gmProgram, &gmVAO);

    // swapping buffers and polling events
    glfwSwapBuffers(window);
    glfwPollEvents();
  }
  // comment
  glfwTerminate();
  return 0;
}
