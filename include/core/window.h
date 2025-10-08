#ifndef WINDOW_H
#define WINDOW_H

#include "glad/glad.h"
#include <GLFW/glfw3.h>

// default values for window (can be modified later)
#define WIDTH 800
#define HEIGHT 600
#define TITLE "Equafun"

// version of OpenGL im using
#define GL_VER_MAJOR 4
#define GL_VER_MINOR 6

// function declarations
void framebufferSizeCallback(GLFWwindow *window, int width, int height);

int initGLFW(void);
int initWindow(GLFWwindow **window);

#endif // WINDOW_H
