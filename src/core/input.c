#include "core/input.h"
#include "core/logger.h"

void processInput(GLFWwindow *window){
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS){
    glfwSetWindowShouldClose(window, true);
    logMsg(DEBUG, "Escape key pressed, closing window...");
  }
}
