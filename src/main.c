#include "expressionEngine/functionManager.h"
#include "glad/glad.h"
#include <GLFW/glfw3.h>

#include "core/logger.h"
#include "core/errorHandler.h"
#include "core/appContext.h"
#include "core/input.h"
#include "textRenderer/text.h"
#include "core/app.h"

#include <string.h>

int main(void){
  // Initialize application context
  struct ra_app_context_t appContext;
  memset(&appContext, 0, sizeof appContext);

  // initialize function manager and add some functions to test drawing
  struct ree_function_manager_t functions;
  char* f = "f(x) = sqrt(x)";
  char* g = "g(x) = x^2 - 3x";
  char* m = "m(x) = ln(x)";
  // factorials DON'T WORK but im too lazy to make a proper factorial function so next commit it is :)

  enum reh_error_code_e err = ree_initFunctionManager(&functions);
  if (err != ERR_SUCCESS){
    ra_appShutdown(&appContext, "Function manager initialization failed.");
    return -1;
  }
  err = ree_addFunction(&functions, f);
  if (err != ERR_SUCCESS){
    ra_appShutdown(&appContext, "Failed to add function f to the function manager.");
    return -1;
  }
  err = ree_addFunction(&functions, g);
  if (err != ERR_SUCCESS){
    ra_appShutdown(&appContext, "Failed to add function g to the function manager.");
    return -1;
  }
  err = ree_addFunction(&functions, m);
  if (err != ERR_SUCCESS){
    ra_appShutdown(&appContext, "Failed to add function l to the function manager.");
    return -1;
  }

  // Initialize application
  err = ra_appInit(&appContext);
  if (err != ERR_SUCCESS){
    ra_appShutdown(&appContext, "Application initialization failed.");
    return -1;
  }

  // Load characters
  struct rtr_character_t characters[ASCII_CHAR_COUNT];
  err = rtr_loadCharactersIntoArray(appContext.face, characters);
  if (err != ERR_SUCCESS){
    ra_appShutdown(&appContext, "Failed to load characters");
    return -1;
  }
  logMsg(SUCCESS, "Characters loaded successfully");

  // Main render loop
  while (!glfwWindowShouldClose(appContext.window)){
    processInput(appContext.window);

    err = ra_appRenderFrame(&appContext, characters, &functions);
    if (err != ERR_SUCCESS){
      ra_appShutdown(&appContext, "Rendering failed.");
      return -1;
    }

    glfwSwapBuffers(appContext.window);
    glfwPollEvents();
  }

  // Clean shutdown
  ra_appShutdown(&appContext, "Application shutting down normally.");
  return 0;
}
