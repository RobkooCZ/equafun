#include "core/logger.h"
#include "glad/glad.h"
#include <GLFW/glfw3.h>

#include "core/window.h"
#include "core/input.h"

int main(void){
  GLFWwindow *window;

  // init glfw and window and make sure it went well
  if (initGLFW() != 0) return -1;
  else logMsg(SUCCESS, "GLFW initialized successfully");

  if (initWindow(&window) != 0) return -1;
  else logMsg(SUCCESS, "Window initialized successfully");

  // render loop
  while (!glfwWindowShouldClose(window)){
    // process input
    processInput(window);

    // render something to prevent segfault
    glClearColor(0.0f, 0.3f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glfwSwapBuffers(window);
    glfwPollEvents();
  }
  // comment
  glfwTerminate();
  return 0;
}
