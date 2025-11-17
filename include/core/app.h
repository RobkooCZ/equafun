/**
  ra - Robkoo's Application
*/

#ifndef APP_H
#define APP_H

#include "core/errorHandler.h"
#include "core/appContext.h"
#include "expressionEngine/functionManager.h"
#include "textRenderer/text.h"

/**
  @brief Initializes the application context
  @returns An error code indicating success or failure
*/
enum reh_error_code_e ra_AppInit(struct ra_app_context_t *ctx);

/**
  @brief Renders a frame using the provided application context, characters, and function manager
  @returns An error code indicating success or failure
*/
enum reh_error_code_e ra_AppRenderFrame(struct ra_app_context_t *ctx, struct rtr_character_t *chars, struct ree_function_manager_t *functions);

/**
  @brief Shuts down the application with an optional message
*/
void ra_AppShutdown(struct ra_app_context_t *ctx, const char *msg);

#endif // APP_H
