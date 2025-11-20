#include "core/input.h"
#include "core/logger.h"
#include "core/window.h"
#include "utils/utilities.h"

bool isXPiLabeled = false;

void rih_ProcessInput(GLFWwindow *window){
  if (window == nullptr){
    rl_LogMsg(RL_ERROR, "Window pointer passed to rih_ProcessInput is NULL.");
    return;
  }

  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS){
    glfwSetWindowShouldClose(window, true);
    rl_LogMsg(RL_DEBUG, "Escape key pressed, closing window...");
    rgu_msleep(200);
  }
  else if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS){
    rl_LogMsg(RL_DEBUG, "P key pressed, changing x-axis labels.");
    isXPiLabeled = !isXPiLabeled;
    redrawWindow = true;
    rl_LogMsg(RL_DEBUG, "Value of isXPiLabeled: %d", isXPiLabeled);
    rgu_msleep(150);
  }
}
