/**
  rwh - Robkoo's Window Handler
*/

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
extern float g_worldXMin;
extern float g_worldXMax;
extern float g_worldYMin;
extern float g_worldYMax;

// variables to hold the current resolution of the window
extern float g_windowWidth;
extern float g_windowHeight;

// flag to tell main if we should rebuild projection matrices
extern bool g_rebuildProjection;
// flag to tell main if we should redraw the window
extern bool g_redrawWindow;

// OpenGL 3.3 due to compatibility
#define GL_VER_MAJOR 3
#define GL_VER_MINOR 3

void rwh_GlfwErrCallback(int errCode, const char* msg);
void rwh_FramebufferSizeCallback(GLFWwindow *window, int width, int height);

enum reh_error_code_e rwh_InitGLFW(void);
enum reh_error_code_e rwh_InitWindow(GLFWwindow **window);

#endif // WINDOW_H
