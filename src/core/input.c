#include "core/input.h"
#include "core/logger.h"

void rih_ProcessInput(GLFWwindow *window){
  if (window == nullptr){
    rl_LogMsg(RL_ERROR, "Window pointer passed to rih_ProcessInput is NULL.");
    return;
  }

  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS){
    glfwSetWindowShouldClose(window, true);
    rl_LogMsg(RL_DEBUG, "Escape key pressed, closing window...");
  }
}
