#include "core/window.h"

#include <GLFW/glfw3.h>
#include <stdio.h>

void framebufferSizeCallback(GLFWwindow *window, int width, int height){
  glViewport(0, 0, width, height);
}

int initGLFW(void){
  if (!glfwInit()){
    fprintf(stderr, "Failed to initialize GLFW.\n");
    return -1; // for now, next version ill add a basic logger
  }
 
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, GL_VER_MAJOR);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, GL_VER_MINOR);

  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  return 0;
}

int initWindow(GLFWwindow **window){
  *window = glfwCreateWindow(WIDTH, HEIGHT, TITLE, NULL, NULL);

  if (*window == nullptr){
    fprintf(stderr, "Failed to create GLFW window.\n");
    return -1;
  }

  glfwMakeContextCurrent(*window);
  glfwSwapInterval(1); // vsync enabled

  if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)){
    fprintf(stderr, "Failed to intialize GLAD.\n");
    return -1;
  }

  glViewport(0, 0, WIDTH, HEIGHT);

  // on resize update width & height
  glfwSetFramebufferSizeCallback(*window, framebufferSizeCallback);

  return 0;
}
