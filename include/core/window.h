#ifndef WINDOW_H
#define WINDOW_H

#include "glad/glad.h"
#include <GLFW/glfw3.h>

#include "core/errorHandler.h"

// default values for window (can be modified later)
#define WIDTH 800
#define HEIGHT 600
#define TITLE "Equafun"

// OpenGL 3.3 due to compatibility
#define GL_VER_MAJOR 3
#define GL_VER_MINOR 3

// function declarations
void framebufferSizeCallback(GLFWwindow *window, int width, int height);

enum reh_error_code_e initGLFW(void);
enum reh_error_code_e initWindow(GLFWwindow **window);

#endif // WINDOW_H
