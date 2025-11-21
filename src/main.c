#include "expressionEngine/functionManager.h"
#include "glad/glad.h"
#include <GLFW/glfw3.h>

#include "core/logger.h"
#include "core/errorHandler.h"
#include "core/appContext.h"
#include "core/input.h"
#include "textRenderer/text.h"
#include "core/app.h"
#include "core/window.h"

#include <string.h>

int main(int argc, char** argv){
  #ifdef _WIN32
    rl_enableANSI();
  #endif

  if (argc > 16 + 1){
    rl_LogMsg(RL_FAILURE, "Too many arguments passed (%d). Max arguments: %d", argc - 1, 16);
    return -1;
  }

  // Initialize application context
  struct ra_app_context_t appContext;
  memset(&appContext, 0, sizeof appContext);

  // initialize function manager and add some functions to test drawing
  struct ree_function_manager_t functions;

  enum reh_error_code_e err = ree_InitFunctionManager(&functions);
  if (err != ERR_SUCCESS){
    ra_AppShutdown(&appContext, "Function manager initialization failed.");
    return -1;
  }

  // based on arguments, dynamically add functions to the manager and render them
  if (argc >= 2){
    int colorIterator = 0;
    for (int i = 1; i < argc; ++i){
      char* fnDef = argv[i];
      err = ree_AddFunction(&functions, fnDef, &functionColorArray[colorIterator]);
      if (err != ERR_SUCCESS){
        ra_AppShutdown(&appContext, "Failed to add function f to the function manager.");
        return -1;
      }
      // increment the color iterator, if its over the array length, put it back to zero
      (colorIterator + 1 > functionColorArrayLength - 1) ? colorIterator = 0 : colorIterator++;
    }
  }
  else {
    ra_AppShutdown(&appContext, "Please run the executable with a function definition as an argument.");
    return -1;
  }
  // factorials DON'T WORK but im too lazy to make a proper factorial function so next commit it is :) (it, infact, was more than one commit)

  // Initialize application
  err = ra_AppInit(&appContext);
  if (err != ERR_SUCCESS){
    ra_AppShutdown(&appContext, "Application initialization failed.");
    return -1;
  }

  // Load characters
  struct rtr_character_t characters[ASCII_CHAR_COUNT];
  err = rtr_LoadCharactersIntoArray(appContext.face, characters);
  if (err != ERR_SUCCESS){
    ra_AppShutdown(&appContext, "Failed to load characters");
    return -1;
  }
  rl_LogMsg(RL_SUCCESS, "Characters loaded successfully");

  // Main render loop
  while (!glfwWindowShouldClose(appContext.window)){
    rih_ProcessInput(appContext.window);

    if (redrawWindow == true){
      err = ra_AppRenderFrame(&appContext, characters, &functions);
      if (err != ERR_SUCCESS){
        ra_AppShutdown(&appContext, "Rendering failed.");
        return -1;
      }
      rl_LogMsg(RL_DEBUG, "Window redraw triggered.");
      glfwSwapBuffers(appContext.window);
      redrawWindow = false;
    }

    glfwPollEvents();
  }

  // Clean shutdown
  ra_AppShutdown(&appContext, "Application shutting down normally.");
  return 0;
}
