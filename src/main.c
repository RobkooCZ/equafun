#include "glad/glad.h"
#include <GLFW/glfw3.h>

#include "core/logger.h"
#include "core/errorHandler.h"
#include "core/appContext.h"
#include "core/input.h"
#include "expressionEngine/parser/functionParser.h"
#include "textRenderer/text.h"
#include "core/app.h"

#include <string.h>

int main(void){
  enum reh_error_code_e err = ree_testLexerParser();
  if (err != ERR_SUCCESS){
    logLastError(ERROR);
    logMsg(FAILURE, "Application cannot continue: Function parser test failed.");
    return -1;
  }
  logMsg(SUCCESS, "Function parser test completed successfully.");
  // TODO: function to call that cleans up all functions' allocated memory
  // rendering of said functions

  // Initialize application context
  struct ra_app_context_t appContext;
  memset(&appContext, 0, sizeof appContext);

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

    err = ra_appRenderFrame(&appContext, characters);
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