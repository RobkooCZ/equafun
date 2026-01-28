
#include "core/input.h"
#include "core/logger.h"
#include "core/window.h"
#include "math/Vec3.h"
#include "renderer/functionRenderer.h"
#include "ui/inputField.h"
#include "ui/ui.h"
#include "ui/uiInternal.h"
#include "utils/utilities.h"

bool isXPiLabeled = false;
bool isUiShown = true;

void rih_ProcessInput(GLFWwindow *window){
  if (window == nullptr){
    rl_LogMsg(RL_ERROR, "Window pointer passed to rih_ProcessInput is NULL.");
    return;
  }

  // get mouse info
  glfwGetCursorPos(window, &ruiState.mouseX, &ruiState.mouseY);
  int leftMouseButton = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);

  if (leftMouseButton == GLFW_PRESS){
    ruiState.mouseDown = 1;
  }
  if (leftMouseButton == GLFW_RELEASE) ruiState.mouseDown = 0;


  if (g_inputFieldData.isAnyPressed == false){
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS){
      glfwSetWindowShouldClose(window, true);
      rl_LogMsg(RL_DEBUG, "Escape key pressed, closing window...");
      rgu_msleep(200);
    }
    else if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS){
      rl_LogMsg(RL_DEBUG, "P key pressed, changing x-axis labels.");
      isXPiLabeled = !isXPiLabeled;
      redrawWindow = true;
      rgu_msleep(150);
    }
    else if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS){
      rl_LogMsg(RL_DEBUG, "R key pressed, changing precision.");
      higherPrecisionRender = !higherPrecisionRender;
      redrawWindow = true;
      rgu_msleep(150);
    }
    else if (glfwGetKey(window, GLFW_KEY_U) == GLFW_PRESS){
      isUiShown = !isUiShown;
      redrawWindow = true;
      rl_LogMsg(RL_DEBUG, "U key pressed, toggling UI...");
      rgu_msleep(150);
    }
  }
  else { // if user has clicked on any input field
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS){
      g_inputFieldData.isAnyPressed = false;
      g_inputFieldData.activeId = 0;
      rgu_msleep(200);
    }
    rui_InputFieldInput(window);
  }
}
