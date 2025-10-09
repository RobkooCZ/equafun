#include "core/window.h"
#include "core/logger.h"

#include <GLFW/glfw3.h>
#include <stdio.h>

void framebufferSizeCallback(GLFWwindow *window, int width, int height){
  glViewport(0, 0, width, height);
  logMsg(DEBUG, "Changing window resolution to: %d, %d", width, height);
}

int initGLFW(void){
  if (!glfwInit()){
    logMsg(FAILURE, "Failed to initialize GLFW.");
    return -1;
  }
 
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, GL_VER_MAJOR);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, GL_VER_MINOR);

  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  return 0;
}

int initWindow(GLFWwindow **window){
  *window = glfwCreateWindow(WIDTH, HEIGHT, TITLE, NULL, NULL);

  if (*window == NULL){
    logMsg(FAILURE, "Failed to create GLFW window.");
    return -1;
  }

  glfwMakeContextCurrent(*window);
  glfwSwapInterval(1); // vsync enabled

  if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)){
    logMsg(FAILURE, "Failed to intialize GLAD.");
    return -1;
  }
  else {
    logMsg(SUCCESS, "GLAD initialized successfully");
  }

  glViewport(0, 0, WIDTH, HEIGHT);

  // on resize update width & height
  glfwSetFramebufferSizeCallback(*window, framebufferSizeCallback);

  return 0;
}
