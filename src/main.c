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
#include "ui/ui.h"
#include "ui/uiInternal.h"
#include "utils/utilities.h"

#ifdef __unix__
#include <signal.h>
#endif

#include <string.h>

int main(int argc, char** argv){
  #ifdef _WIN32
    rl_enableANSI();
  #else
    struct sigaction sa;
    sa.sa_flags = SA_SIGINFO; // additional context
    sa.sa_sigaction = rl_HandleSegfault;
    sigaction(SIGSEGV, &sa, NULL);
  #endif

  // Initialize application context
  struct ra_app_context_t appContext;
  memset(&appContext, 0, sizeof appContext);

  enum reh_error_code_e err = ree_InitFunctionManager(&g_functions);
  if (err != ERR_SUCCESS){
    ra_AppShutdown(&appContext, "Function manager initialization failed.");
    return -1;
  }

  // based on arguments, dynamically add functions to the manager and render them
  if (argc >= 2){
    for (int i = 1; i < argc; ++i){
      char* fnDef = argv[i];
      err = ree_AddFunction(&g_functions, fnDef, &g_functionColorArray[g_colorIterator]);
      if (err != ERR_SUCCESS){
        ra_AppShutdown(&appContext, "Failed to add function f to the function manager.");
        return -1;
      }
      // increment the color iterator, if its over the array length, put it back to zero
      INCREMENT_COLOR_ITERATOR;
    }
  }
  else {
    rl_LogMsg(RL_WARNING, "No arguments passed. Use the input fields to input functions to render.");
  }

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
    err = ra_AppRenderFrame(&appContext, &g_uiCtx, characters, &g_functions);
    if (err != ERR_SUCCESS){
      ra_AppShutdown(&appContext, "Rendering failed.");
      return -1;
    }
    glfwSwapBuffers(appContext.window);
    rgu_msleep(16);
    glfwPollEvents();
  }

  // Clean shutdown
  ra_AppShutdown(&appContext, "Application shutting down normally.");
  return 0;
}
