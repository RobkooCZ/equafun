#include "core/app.h"
#include "glad/glad.h"
#include <GLFW/glfw3.h>

#include "core/logger.h"
#include "core/window.h"
#include "renderer/graph.h"
#include "utils/shaderUtils.h"
#include "math/Mat4.h"
#include "math/Vec3.h"

enum reh_error_code_e ra_appInit(struct ra_app_context_t *ctx){
  if (ctx == nullptr){
    SET_ERROR_RETURN(ERR_INVALID_POINTER, "Context pointer is NULL in ra_appInit()");
  }

  enum reh_error_code_e err;

  // GLFW initialization
  err = initGLFW();
  if (err != ERR_SUCCESS){
    logLastError(ERROR);
    logMsg(FAILURE, "Application cannot continue: GLFW initialization failed.");
    return err;
  }
  logMsg(SUCCESS, "GLFW initialized successfully");

  // Window creation
  err = initWindow(&ctx->window);
  if (err != ERR_SUCCESS){
    logLastError(ERROR);
    logMsg(FAILURE, "Application cannot continue: Window creation failed");
    return err;
  }
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
  if (err != ERR_SUCCESS){
    logLastError(ERROR);
    logMsg(FAILURE, "Application cannot continue: Graph axis rendering system failed to initialize");
    return err;
  }

  err = setupMarkerShaders(&ctx->gmProgram);
  if (err != ERR_SUCCESS){
    logLastError(ERROR);
    logMsg(FAILURE, "Application cannot continue: Graph marker shaders failed to initialize");
    return err;
  }

  err = setupMarkerBuffers(&ctx->gmVAO, &ctx->gmVBO, &ctx->gmEBO);
  if (err != ERR_SUCCESS){
    logLastError(ERROR);
    logMsg(FAILURE, "Application cannot continue: Graph marker buffers failed to initialize");
    return err;
  }

  // Setup projection matrix for the graph and markers
  struct rm_mat4_t graphProjection;
  err = rm_Mat4Ortho(worldXMin, worldXMax, worldYMin, worldYMax, &graphProjection);
  if (err != ERR_SUCCESS){
    logLastError(ERROR);
    logMsg(FAILURE, "Application cannot continue: Failed to create graphProjection matrix");
    return err;
  }

  float *graphProjectionPtr = nullptr;
  err = rm_Mat4ValuePtr(&graphProjection, &graphProjectionPtr);
  if (err != ERR_SUCCESS){
    logLastError(ERROR);
    logMsg(FAILURE, "Application cannot continue: Failed to get graphProjection matrix pointer");
    return err;
  }

  glUseProgram(ctx->gProgram);
  gluSetMat4(ctx->gProgram, "graphProjection", graphProjectionPtr);

  // FreeType initialization
  err = rtr_initFt(&ctx->ft);
  if (err != ERR_SUCCESS){
    logLastError(ERROR);
    logMsg(FAILURE, "Application cannot continue: FreeType initialization failed");
    return err;
  }
  logMsg(SUCCESS, "FreeType initialized successfully");

  // Load font face
  err = rtr_initFtFace(&ctx->ft, &ctx->face);
  if (err != ERR_SUCCESS){
    logLastError(ERROR);
    logMsg(FAILURE, "Application cannot continue: Font face initialization failed");
    return err;
  }
  logMsg(SUCCESS, "Font face initialized successfully");

  // Load text rendering shader sources
  err = loadShaderSource("data/shaders/textRender.vert", &ctx->vertexShaderSrc);
  if (err != ERR_SUCCESS){
    logLastError(ERROR);
    logMsg(FAILURE, "Application cannot continue: Failed to load text vertex shader");
    return err;
  }

  err = loadShaderSource("data/shaders/textColor.frag", &ctx->fragShaderSrc);
  if (err != ERR_SUCCESS){
    logLastError(ERROR);
    logMsg(FAILURE, "Application cannot continue: Failed to load text fragment shader");
    return err;
  }

  err = compileShader(ctx->vertexShaderSrc, GL_VERTEX_SHADER, &ctx->vertexShader);
  if (err != ERR_SUCCESS){
    logLastError(ERROR);
    logMsg(FAILURE, "Application cannot continue: Failed to compile text vertex shader");
    return err;
  }

  err = compileShader(ctx->fragShaderSrc, GL_FRAGMENT_SHADER, &ctx->fragShader);
  if (err != ERR_SUCCESS){
    logLastError(ERROR);
    logMsg(FAILURE, "Application cannot continue: Failed to compile text fragment shader");
    return err;
  }

  err = linkShaders(ctx->vertexShader, ctx->fragShader, &ctx->textProgram);
  if (err != ERR_SUCCESS){
    logLastError(ERROR);
    logMsg(FAILURE, "Application cannot continue: Failed to link text shaders");
    return err;
  }
  logMsg(SUCCESS, "Text rendering shader program created successfully");

  err = rtr_createTextRenderVAO(&ctx->textVAO, &ctx->textVBO);
  if (err != ERR_SUCCESS){
    logLastError(ERROR);
    logMsg(FAILURE, "Application cannot continue: Failed to create text rendering VAO/VBO");
    return err;
  }

  // Setup orthographic projection for text rendering
  struct rm_mat4_t textProjection;
  err = rm_Mat4Ortho(0.0f, WIDTH, 0.0f, HEIGHT, &textProjection);
  if (err != ERR_SUCCESS){
    logLastError(ERROR);
    logMsg(FAILURE, "Application cannot continue: Failed to create textProjection matrix");
    return err;
  }

  float *textProjectionPtr = nullptr;
  err = rm_Mat4ValuePtr(&textProjection, &textProjectionPtr);
  if (err != ERR_SUCCESS){
    logLastError(ERROR);
    logMsg(FAILURE, "Application cannot continue: Failed to get textProjection matrix pointer");
    return err;
  }

  glUseProgram(ctx->textProgram);
  gluSetMat4(ctx->textProgram, "textProjection", textProjectionPtr);

  return ERR_SUCCESS;
}

enum reh_error_code_e ra_appRenderFrame(struct ra_app_context_t *ctx, struct rtr_character_t *chars){
  if (ctx == nullptr){
    SET_ERROR_RETURN(ERR_INVALID_POINTER, "Context pointer is NULL in ra_appRenderFrame()");
  }

  if (chars == nullptr){
    SET_ERROR_RETURN(ERR_INVALID_POINTER, "Characters pointer is NULL in ra_appRenderFrame()");
  }

  enum reh_error_code_e err;

  // Rebuild projection matrices if resolution changed
  if (rebuildProjection == true){
    struct rm_mat4_t graphProjection;
    rm_Mat4Ortho(worldXMin, worldXMax, worldYMin, worldYMax, &graphProjection);

    float *graphProjectionPtr = nullptr;
    rm_Mat4ValuePtr(&graphProjection, &graphProjectionPtr);
    gluSetMat4(ctx->gProgram, "graphProjection", graphProjectionPtr);

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
  if (err != ERR_SUCCESS){
    logLastError(ERROR);
    logMsg(FAILURE, "Application cannot continue: Failed to render graph.");
    return err;
  }

  err = renderMarkers(&ctx->gmProgram, &ctx->gmVAO, &ctx->gmVBO, &ctx->gmEBO, &graphProjectionPtr);
  if (err != ERR_SUCCESS){
    logLastError(ERROR);
    logMsg(FAILURE, "Application cannot continue: Failed to render graph markers.");
    return err;
  }

  // Render text
  struct rm_vec3_t textColor = {1.0f, 1.0f, 1.0f};
  err = rtr_renderAxisLabels(ctx->textProgram, ctx->textVAO, ctx->textVBO, chars, 1.0f, textColor);
  if (err != ERR_SUCCESS){
    logLastError(ERROR);
    logMsg(FAILURE, "Application cannot continue: Failed to render axis labels.");
    return err;
  }

  return ERR_SUCCESS;
}

void ra_appShutdown(struct ra_app_context_t *ctx, const char *msg){
  ra_appContextCleanup(ctx, msg);
}
