/**
  rih - Robkoo's Input Handler
*/

#ifndef INPUT_H
#define INPUT_H

#include "glad/glad.h"
#include <GLFW/glfw3.h>

extern bool isXPiLabeled;
extern bool isUiShown;

/**
  @brief Processes input for the given GLFW window.
*/
void rih_ProcessInput(GLFWwindow *window);

#endif // INPUT_H
