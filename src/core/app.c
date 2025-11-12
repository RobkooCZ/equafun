#include "core/app.h"
#include "core/errorHandler.h"
#include "glad/glad.h"
#include <GLFW/glfw3.h>

#include "core/logger.h"
#include "core/window.h"
#include "renderer/functionRenderer.h"
#include "renderer/graph.h"
#include "utils/shaderUtils.h"
#include "math/Mat4.h"
#include "math/Vec3.h"
#include "math/typeConvert.h"

enum reh_error_code_e ra_appInit(struct ra_app_context_t *ctx){
  if (ctx == nullptr){
    SET_ERROR_RETURN(ERR_INVALID_POINTER, "Context pointer is NULL in ra_appInit()");
  }

  enum reh_error_code_e err;

  // GLFW initialization
  err = initGLFW();
  if (err != ERR_SUCCESS) return err;
  logMsg(SUCCESS, "GLFW initialized successfully");

  // Window creation
  err = initWindow(&ctx->window);
  if (err != ERR_SUCCESS) return err;
  logMsg(SUCCESS, "Window initialized successfully");

  // OpenGL setup
  const GLubyte* version = glGetString(GL_VERSION);
  logMsg(DEBUG, "OpenGL version: %s", version);

  glEnable(GL_LINE_SMOOTH);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glfwSwapInterval(1);

  // Graph resources
  err = setupGraph(&ctx->gProgram, &ctx->gVAO, &ctx->gVBO, &ctx->gEBO);
  if (err != ERR_SUCCESS) return err;

  err = setupMarkerShaders(&ctx->gmProgram);
  if (err != ERR_SUCCESS) return err;

  err = setupMarkerBuffers(&ctx->gmVAO, &ctx->gmVBO, &ctx->gmEBO);
  if (err != ERR_SUCCESS) return err;

  // Setup function resources
  err = rfr_init(ctx);
  if (err != ERR_SUCCESS) return err;

  // Setup projection matrix for the graph and markers
  struct rm_mat4_t graphProjection;
  err = rm_Mat4Ortho(worldXMin, worldXMax, worldYMin, worldYMax, &graphProjection);
  if (err != ERR_SUCCESS) return err;

  float *graphProjectionPtr = nullptr;
  err = rm_Mat4ValuePtr(&graphProjection, &graphProjectionPtr);
  if (err != ERR_SUCCESS) return err;

  glUseProgram(ctx->gProgram);
  gluSetMat4(ctx->gProgram, "graphProjection", graphProjectionPtr);

  // FreeType initialization
  err = rtr_initFt(&ctx->ft);
  if (err != ERR_SUCCESS) return err;
  logMsg(SUCCESS, "FreeType initialized successfully");

  // Load font face
  err = rtr_initFtFace(&ctx->ft, &ctx->face);
  if (err != ERR_SUCCESS) return err;
  logMsg(SUCCESS, "Font face initialized successfully");

  // Load text rendering shader sources
  err = loadShaderSource("data/shaders/textRender.vert", &ctx->vertexShaderSrc);
  if (err != ERR_SUCCESS) return err;

  err = loadShaderSource("data/shaders/textColor.frag", &ctx->fragShaderSrc);
  if (err != ERR_SUCCESS) return err;

  err = compileShader(ctx->vertexShaderSrc, GL_VERTEX_SHADER, &ctx->vertexShader);
  if (err != ERR_SUCCESS) return err;

  err = compileShader(ctx->fragShaderSrc, GL_FRAGMENT_SHADER, &ctx->fragShader);
  if (err != ERR_SUCCESS) return err;

  err = linkShaders(ctx->vertexShader, ctx->fragShader, &ctx->textProgram);
  if (err != ERR_SUCCESS) return err;
  logMsg(SUCCESS, "Text rendering shader program created successfully");

  err = rtr_createTextRenderVAO(&ctx->textVAO, &ctx->textVBO);
  if (err != ERR_SUCCESS) return err;

  // Setup orthographic projection for text rendering
  struct rm_mat4_t textProjection;
  err = rm_Mat4Ortho(0.0f, WIDTH, 0.0f, HEIGHT, &textProjection);
  if (err != ERR_SUCCESS) return err;

  float *textProjectionPtr = nullptr;
  err = rm_Mat4ValuePtr(&textProjection, &textProjectionPtr);
  if (err != ERR_SUCCESS) return err;

  glUseProgram(ctx->textProgram);
  gluSetMat4(ctx->textProgram, "textProjection", textProjectionPtr);

  return ERR_SUCCESS;
}

enum reh_error_code_e ra_appRenderFrame(struct ra_app_context_t *ctx, struct rtr_character_t *chars, struct ree_function_manager_t *functions){
  if (ctx == nullptr){
    SET_ERROR_RETURN(ERR_INVALID_POINTER, "Context pointer is NULL in ra_appRenderFrame()");
  }

  if (chars == nullptr){
    SET_ERROR_RETURN(ERR_INVALID_POINTER, "Characters pointer is NULL in ra_appRenderFrame()");
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
    framebufferSizeCallback(ctx->window, framebufferWidth, framebufferHeight);
    prevFramebufferWidth  = framebufferWidth;
    prevFramebufferHeight = framebufferHeight;
  }

  enum reh_error_code_e err;

  // Rebuild projection matrices if resolution changed
  if (rebuildProjection == true){
    struct rm_mat4_t graphProjection;
    rm_Mat4Ortho(worldXMin, worldXMax, worldYMin, worldYMax, &graphProjection);

    float *graphProjectionPtr = nullptr;
    rm_Mat4ValuePtr(&graphProjection, &graphProjectionPtr);
    gluSetMat4(ctx->gProgram, "graphProjection", graphProjectionPtr);
    gluSetMat4(ctx->fProgram, "functionProjection", graphProjectionPtr);

    struct rm_mat4_t textProjection;
    rm_Mat4Ortho(0.0f, windowWidth, 0.0f, windowHeight, &textProjection);

    float *textProjectionPtr = nullptr;
    rm_Mat4ValuePtr(&textProjection, &textProjectionPtr);
    gluSetMat4(ctx->textProgram, "textProjection", textProjectionPtr);

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
  err = renderGraph(&ctx->gProgram, &ctx->gVAO, &ctx->gVBO, &graphProjectionPtr);
  if (err != ERR_SUCCESS) return err;

  err = renderMarkers(&ctx->gmProgram, &ctx->gmVAO, &ctx->gmVBO, &ctx->gmEBO, &graphProjectionPtr);
  if (err != ERR_SUCCESS) return err;

  err = rfr_render(ctx, functions, &graphProjectionPtr);
  if (err != ERR_SUCCESS) return err;

  glBindVertexArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glUseProgram(0);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  struct rm_vec3_t textColor = {1.0f, 1.0f, 1.0f};
  err = rtr_renderAxisLabels(ctx->textProgram, ctx->textVAO, ctx->textVBO, chars, 1.0f, textColor);
  if (err != ERR_SUCCESS) return err;

  return ERR_SUCCESS;
}

void ra_appShutdown(struct ra_app_context_t *ctx, const char *msg){
  ra_appContextCleanup(ctx, msg);
}