#ifndef WINDOW_H
#define WINDOW_H

#include "glad/glad.h"
#include <GLFW/glfw3.h>

#include "core/errorHandler.h"

// default values for window
#define WIDTH  800
#define HEIGHT 600
#define ASPECT_RATIO ((float)WIDTH / (float)HEIGHT)
#define TITLE "Equafun"

// variables to hold the boundaries of the world space
extern float worldXMin;
extern float worldXMax;
extern float worldYMin;
extern float worldYMax;

// variables to hold the current resolution of the window
extern float windowWidth;
extern float windowHeight;

// flag to tell main if we should rebuild projection matrices
extern bool rebuildProjection;
// flag to tell main if we should redraw the window
extern bool redrawWindow;

// OpenGL 3.3 due to compatibility
#define GL_VER_MAJOR 3
#define GL_VER_MINOR 3

void glfwErrCallback(int errCode, const char* msg);
void framebufferSizeCallback(GLFWwindow *window, int width, int height);

enum reh_error_code_e initGLFW(void);
enum reh_error_code_e initWindow(GLFWwindow **window);

#endif // WINDOW_H
