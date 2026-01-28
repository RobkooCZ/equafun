#include "ui/ui.h"
#include "core/errorHandler.h"
#include "ui/inputField.h"
#include "ui/uiInternal.h"
#include "utils/renderUtils.h"
#include "utils/shaderUtils.h"
#include "utils/utilities.h"
#include <stdlib.h>
#include <string.h>
#include "core/logger.h"

struct rui_context_t uiCtx;
struct rui_state_t ruiState = {0, 0, 0, 0, 0};

enum reh_error_code_e rui_Begin(struct rui_context_t *uiCtx){
  if (uiCtx == nullptr) SET_ERROR_RETURN(ERR_INVALID_POINTER, "Pointer to uiCtx passed to rui_Begin is NULL.");
  // reset the context
  memset(uiCtx->commands, 0, sizeof(struct rui_command_t) * uiCtx->commandCount);
  uiCtx->commandCount = 0;

  return ERR_SUCCESS;
}

enum reh_error_code_e rui_RenderRect(struct rui_context_t *uiCtx, float x, float y, const char* label, float w, float h, struct rm_vec3_t color, void (*onHover)(struct rui_command_rect_t *), void (*onClick)(struct rui_command_rect_t *)){
  if (x < 0) SET_ERROR_RETURN(ERR_INVALID_INPUT, "Negative x passed to rui_RenderRect.");
  if (y < 0) SET_ERROR_RETURN(ERR_INVALID_INPUT, "Negative y passed to rui_RenderRect.");
  if (w < 0) SET_ERROR_RETURN(ERR_INVALID_INPUT, "Negative w passed to rui_RenderRect.");
  if (h < 0) SET_ERROR_RETURN(ERR_INVALID_INPUT, "Negative h passed to rui_RenderRect.");
  if (label == nullptr) SET_ERROR_RETURN(ERR_INVALID_POINTER, "Pointer to label passed to rui_RenderRect is NULL.");
  if (uiCtx == nullptr) SET_ERROR_RETURN(ERR_INVALID_POINTER, "Pointer to uiCtx passed to rui_RenderRect is NULL.");
  if (uiCtx->commandCount >= RUI_MAX_COMMANDS) SET_ERROR_RETURN(ERR_OUT_OF_BOUNDS, "Too many commands in uiCtx (passed to rui_RenderRect).");

  struct rui_command_t *command = &uiCtx->commands[uiCtx->commandCount++];

  command->id = rgu_hash(label) ^ (int)x;

  command->type = RUI_RECT;
  command->rect.x = x;
  command->rect.y = y;
  command->rect.w = w;
  command->rect.h = h;
  command->rect.color = color;
  command->rect.onHover = onHover;
  command->rect.onClick = onClick;

  return ERR_SUCCESS;
}

enum reh_error_code_e rui_RenderInputField(struct rui_context_t *uiCtx, float x, float y, const char* label, float w, float h, float borderSize, float innerPadding, struct rm_vec3_t color, struct rm_vec3_t borderColor, void (*onClick)(struct rui_command_input_field_t *)){
  if (x < 0) SET_ERROR_RETURN(ERR_INVALID_INPUT, "Negative x passed to rui_RenderRect.");
  if (y < 0) SET_ERROR_RETURN(ERR_INVALID_INPUT, "Negative y passed to rui_RenderRect.");
  if (w < 0) SET_ERROR_RETURN(ERR_INVALID_INPUT, "Negative w passed to rui_RenderRect.");
  if (h < 0) SET_ERROR_RETURN(ERR_INVALID_INPUT, "Negative h passed to rui_RenderRect.");
  if (label == nullptr) SET_ERROR_RETURN(ERR_INVALID_POINTER, "Pointer to label passed to rui_RenderRect is NULL.");
  if (uiCtx == nullptr) SET_ERROR_RETURN(ERR_INVALID_POINTER, "Pointer to uiCtx passed to rui_RenderRect is NULL.");
  if (uiCtx->commandCount >= RUI_MAX_COMMANDS) SET_ERROR_RETURN(ERR_OUT_OF_BOUNDS, "Too many commands in uiCtx (passed to rui_RenderRect).");

  struct rui_command_t *command = &uiCtx->commands[uiCtx->commandCount++];

  struct rui_input_cursor_data_t cursor = {0.0f, 0};
  struct rui_input_field_data_t inputField = {
    .isInitialized = 1,
    .cursor = &cursor,
    .inputX = 0.0f,
    .inputY = 0.0f,
    .textScale = 0.0f,
    .inputText = (char*)malloc(INPUT_FIELD_CHAR_CAP * sizeof(char)),
    .charCount = 0
  };

  if (inputField.inputText == nullptr) SET_ERROR_RETURN(ERR_OUT_OF_MEMORY, "Failed to allocate space for inputText");
  strcpy(inputField.inputText, "");

  command->id = rgu_hash(label) ^ (int)x;
  command->type = RUI_INPUT_FIELD;
  command->input.x = x;
  command->input.y = y;
  command->input.w = w;
  command->input.h = h;

  if (g_inputFields[uiCtx->commandCount - 1].data.isInitialized == 0){
    g_inputFields[uiCtx->commandCount - 1].id = command->id;
    g_inputFields[uiCtx->commandCount - 1].data = inputField;
  }

  command->input.innerPadding = innerPadding;

  command->input.color = color;
  command->input.borderColor = borderColor;
  command->input.borderSize = borderSize;

  command->input.onClick = onClick;
  return ERR_SUCCESS;
}

enum reh_error_code_e rui_DrawRect(float x, float y, float w, float h, struct rm_vec3_t color, struct rui_draw_data_t *drawData){
  if (drawData == nullptr){
    SET_ERROR_RETURN(ERR_INVALID_POINTER, "Pointer to drawData in rui_DrawRect is NULL");
  }

  // convert top left y (my ui lib)
  extern float windowHeight;
  float topYGL = windowHeight - y;
  float bottomYGL = topYGL - h;

  // top left (converted)
  drawData->vertices[drawData->verticesCount++] = x;
  drawData->vertices[drawData->verticesCount++] = topYGL;
  drawData->vertices[drawData->verticesCount++] = drawData->commandCount / 10000.0f;

  drawData->vertices[drawData->verticesCount++] = color.x;
  drawData->vertices[drawData->verticesCount++] = color.y;
  drawData->vertices[drawData->verticesCount++] = color.z;

  // top right
  drawData->vertices[drawData->verticesCount++] = x + w;
  drawData->vertices[drawData->verticesCount++] = topYGL;
  drawData->vertices[drawData->verticesCount++] = drawData->commandCount / 10000.0f;

  drawData->vertices[drawData->verticesCount++] = color.x;
  drawData->vertices[drawData->verticesCount++] = color.y;
  drawData->vertices[drawData->verticesCount++] = color.z;

  // bottom left
  drawData->vertices[drawData->verticesCount++] = x;
  drawData->vertices[drawData->verticesCount++] = bottomYGL;
  drawData->vertices[drawData->verticesCount++] = drawData->commandCount / 10000.0f;

  drawData->vertices[drawData->verticesCount++] = color.x;
  drawData->vertices[drawData->verticesCount++] = color.y;
  drawData->vertices[drawData->verticesCount++] = color.z;

  // bottom right
  drawData->vertices[drawData->verticesCount++] = x + w;
  drawData->vertices[drawData->verticesCount++] = bottomYGL;
  drawData->vertices[drawData->verticesCount++] = drawData->commandCount / 10000.0f;

  drawData->vertices[drawData->verticesCount++] = color.x;
  drawData->vertices[drawData->verticesCount++] = color.y;
  drawData->vertices[drawData->verticesCount++] = color.z;

  size_t base = drawData->vertexCount;

  // triangle 1
  drawData->indices[drawData->indicesCount++] = (unsigned int)(base + 0);
  drawData->indices[drawData->indicesCount++] = (unsigned int)(base + 1);
  drawData->indices[drawData->indicesCount++] = (unsigned int)(base + 2);

  // triangle 2
  drawData->indices[drawData->indicesCount++] = (unsigned int)(base + 2);
  drawData->indices[drawData->indicesCount++] = (unsigned int)(base + 1);
  drawData->indices[drawData->indicesCount++] = (unsigned int)(base + 3);

  drawData->vertexCount += 4;

  return ERR_SUCCESS;
}

enum reh_error_code_e rui_DrawInputField(float x, float y, float w, float h, float borderSize, struct rm_vec3_t color, struct rm_vec3_t borderColor, struct rui_draw_data_t *drawData){
  if (drawData == nullptr){
    SET_ERROR_RETURN(ERR_INVALID_POINTER, "Pointer to drawData in rui_DrawInputFIeld is NULL");
  }

  // "border"
  rui_DrawRect(x - borderSize, y - borderSize, w + (borderSize * 2.0f), h + (borderSize * 2.0f), borderColor, drawData);

  rui_DrawRect(x, y, w, h, color, drawData);

  return ERR_SUCCESS;
}


enum reh_error_code_e rui_SetupRenderData(GLuint *program, GLuint *VAO, GLuint *VBO, GLuint *EBO){
  char* vertexShaderSrc = nullptr;
  char* fragmentShaderSrc = nullptr;

  CHECK_ERROR_CTX(rsu_LoadShaderSource("data/shaders/ui.vert", &vertexShaderSrc), "Failed to load vertex shader for UI");

  CHECK_ERROR_CTX(rsu_LoadShaderSource("data/shaders/ui.frag", &fragmentShaderSrc), "Failed to load fragment shader for UI");

  GLuint vertexShader = 0;
  GLuint fragShader = 0;

  enum reh_error_code_e err = rsu_CompileShader(vertexShaderSrc, GL_VERTEX_SHADER, &vertexShader);
  if (err != ERR_SUCCESS){
    free(vertexShaderSrc);
    free(fragmentShaderSrc);
    ADD_ERROR_CONTEXT_RETURN(err, "Failed to compile vertex shader for UI");
  }

  err = rsu_CompileShader(fragmentShaderSrc, GL_FRAGMENT_SHADER, &fragShader);
  if (err != ERR_SUCCESS){
    free(vertexShaderSrc);
    free(fragmentShaderSrc);
    glDeleteShader(vertexShader);
    ADD_ERROR_CONTEXT_RETURN(err, "Failed to compile fragment shader for UI");
  }

  err = rsu_LinkShaders(vertexShader, fragShader, program);
  free(vertexShaderSrc);
  free(fragmentShaderSrc);

  if (err != ERR_SUCCESS){
    ADD_ERROR_CONTEXT_RETURN(err, "Failed to link shaders for UI program");
  }

  size_t maxVertices = 3 * 2 * 2 * 4 * RUI_MAX_COMMANDS;
  size_t maxIndices  = 6 * 2 * RUI_MAX_COMMANDS;

  float *vertices = (float *)malloc(sizeof(float) * maxVertices);
  if (vertices == nullptr){
    SET_ERROR_RETURN(ERR_OUT_OF_MEMORY, "Failed to allocate temporary vertices in rui_SetupRenderData()");
  }

  GLuint *indices = (GLuint *)malloc(sizeof(GLuint) * maxIndices);
  if (indices == nullptr){
    free(vertices);
    SET_ERROR_RETURN(ERR_OUT_OF_MEMORY, "Failed to allocate temporary indices in rui_SetupRenderData()");
  }
 
  enum reh_error_code_e renderErr = rru_SetupRenderData(vertices, sizeof(float) * maxVertices, indices, sizeof(GLuint) * maxIndices, VAO, VBO, EBO, true);
  free(vertices);
  free(indices);

  if (renderErr != ERR_SUCCESS){
    ADD_ERROR_CONTEXT_RETURN(renderErr, "Failed to setup render data for UI");
  }

  return ERR_SUCCESS;
}

enum reh_error_code_e rui_End(struct rui_context_t *uiCtx, GLuint *program, GLuint *VAO, GLuint *VBO, GLuint *EBO, float **projectionMatrixPtr){
  struct rui_draw_data_t drawData;

  // multiply by two due to input fields being two rectangles
  drawData.vertices = malloc(sizeof(float) * 3 * 2 * 4 * uiCtx->commandCount * 2);
  drawData.indices = malloc(sizeof(unsigned int) * 6 * uiCtx->commandCount * 2);

  drawData.commandCount = 0;
  drawData.vertexCount = 0;
  drawData.indicesCount = 0;
  drawData.verticesCount = 0;

  for (size_t i = 0; i < uiCtx->commandCount; ++i){
    struct rui_command_t currentCommand = uiCtx->commands[i];
    drawData.commandCount = i;
    switch (currentCommand.type){
      case RUI_RECT:
        rui_DrawRect(currentCommand.rect.x, currentCommand.rect.y, currentCommand.rect.w, currentCommand.rect.h, currentCommand.rect.color, &drawData);
        break;
      case RUI_INPUT_FIELD:
        rui_DrawInputField(currentCommand.input.x, currentCommand.input.y, currentCommand.input.w, currentCommand.input.h, currentCommand.input.borderSize, currentCommand.input.color, currentCommand.input.borderColor, &drawData);
        break;
      default: break;
    }
  }

  size_t verticesBytes = drawData.verticesCount * sizeof(float);
  size_t indicesBytes = drawData.indicesCount * sizeof(unsigned int);

  glBindVertexArray(*VAO);
  glBindBuffer(GL_ARRAY_BUFFER, *VBO);
  glBufferSubData(GL_ARRAY_BUFFER, 0, verticesBytes, drawData.vertices);
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, *EBO);
  glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, indicesBytes, drawData.indices);

  glUseProgram(*program);
  rsu_GluSetMat4(*program, "uiProjection", *projectionMatrixPtr);
  glBindVertexArray(*VAO);
  glLineWidth(2.0f);
  glDrawElements(GL_TRIANGLES, (GLsizei)drawData.indicesCount, GL_UNSIGNED_INT, 0);
  glBindVertexArray(0);

  return ERR_SUCCESS;
}

enum reh_error_code_e rui_AABBCollisionCheck(struct rui_command_t *A, struct rui_command_t *B, bool *collision){
  if (A == nullptr){
    SET_ERROR_RETURN(ERR_INVALID_POINTER, "Pointer to A in rui_AABBCollisionCheck is NULL");
  }
  if (B == nullptr){
    SET_ERROR_RETURN(ERR_INVALID_POINTER, "Pointer to B in rui_AABBCollisionCheck is NULL");
  }
  if (collision == nullptr){
    SET_ERROR_RETURN(ERR_INVALID_POINTER, "Pointer to collision in rui_AABBCollisionCheck is NULL");
  }
  // declare the points so we can fill them with the appropriate data then
  float xA, yA, wA, hA, xB, yB, wB, hB;
  float innerPadding = 0;
  switch (A->type){
    case RUI_RECT:
      xA = A->rect.x;
      yA = A->rect.y;
      wA = A->rect.w;
      hA = A->rect.y;
      break;
    case RUI_INPUT_FIELD:
      xA = A->input.x;
      yA = A->input.y;
      wA = A->input.w;
      hA = A->input.y;
      innerPadding = A->input.innerPadding;
      break;
    default:
      SET_ERROR_RETURN(ERR_INVALID_INPUT, "Invalid command (A) type passed to rui_AABBCollisionCheck.");
  }

  switch (B->type){
    case RUI_RECT:
      xB = B->rect.x;
      yB = B->rect.y;
      wB = B->rect.w;
      hB = B->rect.y;
      break;
    case RUI_INPUT_FIELD:
      xB = B->input.x;
      yB = B->input.y;
      wB = B->input.w;
      hB = B->input.y;
      innerPadding = B->input.innerPadding;
      break;
    default:
      SET_ERROR_RETURN(ERR_INVALID_INPUT, "Invalid command (B) type passed to rui_AABBCollisionCheck.");
  }

  // for an input field, we wanna check collision with the inner field, not the border, so we have to take into account the innerPadding. (soon TM)
  bool AisToTheRightOfB = xA > (xB + wB);
  bool AisToTheLeftOfB = (xA + wA) < xB;
  bool AisAboveB = (yA + hA) < yB;
  bool AisBelowB = yA > (yB + hB);

  *collision = !(AisToTheRightOfB || AisToTheLeftOfB || AisAboveB || AisBelowB);

  return ERR_SUCCESS;
}

enum reh_error_code_e rui_AACursorCheck(int *id){
  // start at the end of the array to check widgets that could be on top of other widgets
  for (size_t i = uiCtx.commandCount; i >= 0; --i){
    struct rui_command_t *command = &uiCtx.commands[i];
    bool isCursorInsideX = false, isCursorInsideY = false;
    if (ruiState.mouseX > command->rect.x && ruiState.mouseX < command->rect.x + command->rect.w) isCursorInsideX = true;

    if (ruiState.mouseY > command->rect.y && ruiState.mouseY < command->rect.y + command->rect.h) isCursorInsideY = true;

    if (isCursorInsideX && isCursorInsideY){
      *id = command->id;
      return ERR_SUCCESS;
    }
  }

  return ERR_SUCCESS;
}
