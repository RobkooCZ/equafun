#include "core/window.h"
#include "core/logger.h"
#include "core/errorHandler.h"

#include <stdio.h>

void framebufferSizeCallback(GLFWwindow *window, int width, int height){
  glViewport(0, 0, width, height);
  logMsg(DEBUG, "Changing window resolution to: %d, %d", width, height);
}

enum reh_error_code_e initGLFW(void){
  if (!glfwInit()){
    SET_ERROR_RETURN(ERR_GLFW_INIT_FAILED, "Failed to initialize GLFW library");
  }
 
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, GL_VER_MAJOR);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, GL_VER_MINOR);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

  return ERR_SUCCESS;
}

enum reh_error_code_e initWindow(GLFWwindow **window){
  if (!window){
    SET_ERROR_RETURN(ERR_INVALID_POINTER, "Window pointer is NULL in initWindow()");
  }

  *window = glfwCreateWindow(WIDTH, HEIGHT, TITLE, nullptr, nullptr);

  if (*window == nullptr){
    SET_ERROR_RETURN(ERR_WINDOW_CREATE_FAILED, "Failed to create GLFW window (width: %d, height: %d, title: '%s')", WIDTH, HEIGHT, TITLE);
  }

  glfwMakeContextCurrent(*window);
  glfwSwapInterval(1); // vsync enabled

  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)){
    glfwDestroyWindow(*window);
    *window = nullptr;
    SET_ERROR_RETURN(ERR_GLAD_INIT_FAILED, "Failed to initialize GLAD");
  }

  // Check OpenGL context is valid
  GLenum glErr = glGetError();
  if (glErr != GL_NO_ERROR){
    char technical[256];
    snprintf(technical, sizeof(technical), "OpenGL error after GLAD initialization: 0x%04X", glErr);
    glfwDestroyWindow(*window);
    *window = nullptr;
    SET_ERROR_TECHNICAL_RETURN(ERR_GLAD_INIT_FAILED, "OpenGL context initialization failed", technical);
  }

  glViewport(0, 0, WIDTH, HEIGHT);

  // Check if viewport setup succeeded
  glErr = glGetError();
  if (glErr != GL_NO_ERROR){
    char technical[256];
    snprintf(technical, sizeof(technical), "glViewport failed with error: 0x%04X", glErr);
    glfwDestroyWindow(*window);
    *window = nullptr;
    SET_ERROR_TECHNICAL_RETURN(ERR_WINDOW_CREATE_FAILED, "Failed to set OpenGL viewport", technical);
  }

  // on resize update width & height
  glfwSetFramebufferSizeCallback(*window, framebufferSizeCallback);

  return ERR_SUCCESS;
}