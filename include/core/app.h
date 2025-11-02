#ifndef APP_H
#define APP_H

#include "core/errorHandler.h"
#include "core/appContext.h"
#include "textRenderer/text.h"

enum reh_error_code_e ra_appInit(struct ra_app_context_t *ctx);
enum reh_error_code_e ra_appRenderFrame(struct ra_app_context_t *ctx, struct rtr_character_t *chars);
void ra_appShutdown(struct ra_app_context_t *ctx, const char *msg);

#endif // APP_H