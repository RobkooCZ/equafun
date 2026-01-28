#include "core/app.h"
#include "core/errorHandler.h"
#include "core/input.h"
#include "glad/glad.h"
#include <GLFW/glfw3.h>

#include "core/logger.h"
#include "core/window.h"
#include "renderer/functionRenderer.h"
#include "renderer/graph.h"
#include "textRenderer/text.h"
#include "ui/inputField.h"
#include "ui/ui.h"
#include "ui/uiInternal.h"
#include "utils/shaderUtils.h"
#include "math/Mat4.h"
#include "math/Vec3.h"
#include "math/typeConvert.h"
#include "ui/uiRectOnEvent.h"

enum reh_error_code_e ra_AppInit(struct ra_app_context_t *ctx){
  if (ctx == nullptr){
    SET_ERROR_RETURN(ERR_INVALID_POINTER, "Context pointer is NULL in ra_AppInit()");
  }

  enum reh_error_code_e err;

  // GLFW initialization
  err = rwh_InitGLFW();
  if (err != ERR_SUCCESS) return err;
  rl_LogMsg(RL_SUCCESS, "GLFW initialized successfully");

  // Window creation
  err = rwh_InitWindow(&ctx->window);
  if (err != ERR_SUCCESS) return err;
  rl_LogMsg(RL_SUCCESS, "Window initialized successfully");

  // OpenGL setup
  const GLubyte* version = glGetString(GL_VERSION);
  rl_LogMsg(RL_DEBUG, "OpenGL version: %s", version);

  glEnable(GL_LINE_SMOOTH);
  glEnable(GL_BLRL_END);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glfwSwapInterval(1);

  // Graph resources
  err = rgr_SetupGraph(&ctx->gProgram, &ctx->gVAO, &ctx->gVBO, &ctx->gEBO);
  if (err != ERR_SUCCESS) return err;

  err = rgr_SetupMarkerShaders(&ctx->gmProgram);
  if (err != ERR_SUCCESS) return err;

  err = rgr_SetupMarkerBuffers(&ctx->gmVAO, &ctx->gmVBO, &ctx->gmEBO);
  if (err != ERR_SUCCESS) return err;

  // Setup UI resources
  err = rui_SetupRenderData(&ctx->uiProgram, &ctx->uiVAO, &ctx->uiVBO, &ctx->uiEBO);
  if (err != ERR_SUCCESS) return err;

  // Setup function resources
  err = rfr_Init(ctx);
  if (err != ERR_SUCCESS) return err;

  // Setup projection matrix for the graph and markers
  struct rm_mat4_t graphProjection;
  err = rm_Mat4Ortho(worldXMin, worldXMax, worldYMin, worldYMax, &graphProjection);
  if (err != ERR_SUCCESS) return err;

  float *graphProjectionPtr = nullptr;
  err = rm_Mat4ValuePtr(&graphProjection, &graphProjectionPtr);
  if (err != ERR_SUCCESS) return err;

  glUseProgram(ctx->gProgram);
  rsu_GluSetMat4(ctx->gProgram, "graphProjection", graphProjectionPtr);

  // FreeType initialization
  err = rtr_InitFt(&ctx->ft);
  if (err != ERR_SUCCESS) return err;
  rl_LogMsg(RL_SUCCESS, "FreeType initialized successfully");

  // Load font face
  err = rtr_InitFtFace(&ctx->ft, &ctx->face);
  if (err != ERR_SUCCESS) return err;
  rl_LogMsg(RL_SUCCESS, "Font face initialized successfully");

  // Load text rendering shader sources
  err = rsu_LoadShaderSource("data/shaders/textRender.vert", &ctx->vertexShaderSrc);
  if (err != ERR_SUCCESS) return err;

  err = rsu_LoadShaderSource("data/shaders/textColor.frag", &ctx->fragShaderSrc);
  if (err != ERR_SUCCESS) return err;

  err = rsu_CompileShader(ctx->vertexShaderSrc, GL_VERTEX_SHADER, &ctx->vertexShader);
  if (err != ERR_SUCCESS) return err;

  err = rsu_CompileShader(ctx->fragShaderSrc, GL_FRAGMENT_SHADER, &ctx->fragShader);
  if (err != ERR_SUCCESS) return err;

  err = rsu_LinkShaders(ctx->vertexShader, ctx->fragShader, &ctx->textProgram);
  if (err != ERR_SUCCESS) return err;
  rl_LogMsg(RL_SUCCESS, "Text rendering shader program created successfully");

  err = rtr_CreateTextRenderVAO(&ctx->textVAO, &ctx->textVBO);
  if (err != ERR_SUCCESS) return err;

  // Setup orthographic projection for text rendering
  struct rm_mat4_t textProjection;
  err = rm_Mat4Ortho(0.0f, WIDTH, 0.0f, HEIGHT, &textProjection);
  if (err != ERR_SUCCESS) return err;

  float *textProjectionPtr = nullptr;
  err = rm_Mat4ValuePtr(&textProjection, &textProjectionPtr);
  if (err != ERR_SUCCESS) return err;

  glUseProgram(ctx->textProgram);
  rsu_GluSetMat4(ctx->textProgram, "textProjection", textProjectionPtr);

  return ERR_SUCCESS;
}

enum reh_error_code_e ra_AppRenderFrame(struct ra_app_context_t *ctx, struct rui_context_t *uiCtx, struct rtr_character_t *chars, struct ree_function_manager_t *functions){
  if (ctx == nullptr){
    SET_ERROR_RETURN(ERR_INVALID_POINTER, "Context pointer is NULL in ra_AppRenderFrame()");
  }

  if (chars == nullptr){
    SET_ERROR_RETURN(ERR_INVALID_POINTER, "Characters pointer is NULL in ra_AppRenderFrame()");
  }

  // hyprland issue
  // https://github.com/glfw/glfw/issues/2768
  // workaround: manually check for framebuffer size changes
  int framebufferWidth = 0;
  int framebufferHeight = 0;
  glfwGetFramebufferSize(ctx->window, &framebufferWidth, &framebufferHeight);

  static int prevFramebufferWidth = 0;
  static int prevFramebufferHeight = 0;
  if (framebufferWidth != prevFramebufferWidth || framebufferHeight != prevFramebufferHeight) {
    rwh_FramebufferSizeCallback(ctx->window, framebufferWidth, framebufferHeight);
    prevFramebufferWidth  = framebufferWidth;
    prevFramebufferHeight = framebufferHeight;
  }

  enum reh_error_code_e err;

  // Rebuild projection matrices and redraw window if resolution changed
  if (rebuildProjection == true){
    struct rm_mat4_t graphProjection;
    rm_Mat4Ortho(worldXMin, worldXMax, worldYMin, worldYMax, &graphProjection);

    float *graphProjectionPtr = nullptr;
    rm_Mat4ValuePtr(&graphProjection, &graphProjectionPtr);
    rsu_GluSetMat4(ctx->gProgram, "graphProjection", graphProjectionPtr);
    rsu_GluSetMat4(ctx->fProgram, "functionProjection", graphProjectionPtr);

    struct rm_mat4_t textProjection;
    rm_Mat4Ortho(0.0f, windowWidth, 0.0f, windowHeight, &textProjection);

    float* textProjectionPtr = nullptr;
    rm_Mat4ValuePtr(&textProjection, &textProjectionPtr);
    rsu_GluSetMat4(ctx->textProgram, "textProjection", textProjectionPtr);

    rebuildProjection = false;
  }

  // Clear buffers
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // Get current projection matrix
  float *graphProjectionPtr = nullptr;
  struct rm_mat4_t graphProjection;
  rm_Mat4Ortho(worldXMin, worldXMax, worldYMin, worldYMax, &graphProjection);
  rm_Mat4ValuePtr(&graphProjection, &graphProjectionPtr);

  // Render graph
  err = rgr_RenderGraph(&ctx->gProgram, &ctx->gVAO, &ctx->gVBO, &graphProjectionPtr);
  if (err != ERR_SUCCESS) return err;

  err = rgr_RenderMarkers(&ctx->gmProgram, &ctx->gmVAO, &ctx->gmVBO, &ctx->gmEBO, &graphProjectionPtr);
  if (err != ERR_SUCCESS) return err;

  err = rfr_Render(ctx, functions, &graphProjectionPtr);
  if (err != ERR_SUCCESS) return err;

  glBindVertexArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glUseProgram(0);
  glEnable(GL_BLRL_END);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  struct rm_vec3_t textColor = {1.0f, 1.0f, 1.0f};
  err = rtr_RenderAxisLabels(ctx->textProgram, ctx->textVAO, ctx->textVBO, chars, 1.0f, textColor);

  if (err != ERR_SUCCESS) return err;
  err = rtr_RenderDebugInfo(ctx->textProgram, ctx->textVAO, ctx->textVBO, chars, 1.0f, textColor, functions);

  if (isUiShown){
    rui_Begin(uiCtx);

    rui_RenderRect(uiCtx, 10, 10, "a", 150, 500, (struct rm_vec3_t){1.0f, 1.0f, 1.0f}, rui_RectOnHoverGreen, rui_RectOnClickBlue);
    rui_RenderRect(uiCtx, 165, 10, "yo", 500, 100, (struct rm_vec3_t){1.0f, 1.0f, 1.0f}, rui_RectOnHoverRed, rui_RectOnClickGray);
    rui_RenderRect(uiCtx, 70, 255, "square", 100, 100, (struct rm_vec3_t){0.0f, 0.0f, 1.0f}, rui_RectOnHoverGray, rui_RectOnClickGreen);

    rui_RenderInputField(uiCtx, 100, 300, "f input", 300, 100, 5, 5, (struct rm_vec3_t){0.0f, 0.0f, 1.0f},(struct rm_vec3_t){1.0f, 0.0f, 0.0f}, rui_InputFieldOnClick);
    rui_RenderInputField(uiCtx, 100, 100, "l input", 200, 100, 5, 5, (struct rm_vec3_t){0.0f, 0.0f, 1.0f},(struct rm_vec3_t){1.0f, 0.0f, 0.0f}, rui_InputFieldOnClick);

    int id;
    rui_AACursorCheck(&id);

    // i ought to optimize this (TODO)
    if (g_inputFieldData.isAnyPressed == true){
      for (size_t i = 0; i < uiCtx->commandCount; i++){
        struct rui_command_t *currentCommand = &uiCtx->commands[i];
        if (currentCommand->type == RUI_INPUT_FIELD && currentCommand->id == g_inputFieldData.activeId){
          currentCommand->input.onClick(&currentCommand->input);
        }
      }
    }

    if (ruiState.mouseDown){
      ruiState.activeItem = id;
      for (size_t i = 0; i < uiCtx->commandCount; i++){
        struct rui_command_t *currentCommand = &uiCtx->commands[i];

        switch (currentCommand->type){
          case RUI_RECT:
            if (ruiState.activeItem == currentCommand->id) currentCommand->rect.onClick(&currentCommand->rect);
            break;
          case RUI_INPUT_FIELD:
            if (ruiState.activeItem == currentCommand->id){
              currentCommand->input.onClick(&currentCommand->input);
              g_inputFieldData.activeId = currentCommand->id;
            }
          default:
            break;
        }
      }
    }
    else {
      ruiState.hotItem = id;

      for (size_t i = 0; i < uiCtx->commandCount; i++){
        struct rui_command_t *currentCommand = &uiCtx->commands[i];

        switch (currentCommand->type){
          case RUI_RECT: 
            if (ruiState.hotItem == currentCommand->id) currentCommand->rect.onHover(&currentCommand->rect);
            break;
          default:
            break;
        }
      }
    }
  }

  struct rm_mat4_t textProjection;
  rm_Mat4Ortho(0.0f, windowWidth, 0.0f, windowHeight, &textProjection);

  float* textProjectionPtr = nullptr;
  rm_Mat4ValuePtr(&textProjection, &textProjectionPtr);
  rsu_GluSetMat4(ctx->textProgram, "textProjection", textProjectionPtr);

  if (isUiShown){
    // first render the ui
    rui_End(uiCtx, &ctx->uiProgram, &ctx->uiVAO, &ctx->uiVBO, &ctx->uiEBO, &textProjectionPtr);

    // then the text on top of it
    for (size_t i = 0; i < uiCtx->commandCount; i++){
      struct rui_command_t *currentCommand = &uiCtx->commands[i];
      if (currentCommand->type == RUI_INPUT_FIELD){
        rui_RenderInputFieldText(&currentCommand->input, ctx, chars, g_inputFields[i].data.inputText);
      }
    }
  }

  return ERR_SUCCESS;
}

void ra_AppShutdown(struct ra_app_context_t *ctx, const char *msg){
  ra_AppContextCleanup(ctx, msg);
}
