#include "glad/glad.h"
#include <GLFW/glfw3.h>

#include "core/window.h"

int main(void){
  GLFWwindow *window;

  if (initGLFW() != 0){
    return -1;
  }

  if (initWindow(&window) != 0){
    return -1;
  }

  // render loop
  while (!glfwWindowShouldClose(window)){
    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  glfwTerminate();
  return 0;
}
