/*
  rui - Robkoo's UI
*/

#ifndef   RUI_H
#define   RUI_H

#include "glad/glad.h"
#include <GLFW/glfw3.h>

#include "core/errorHandler.h"
#include "math/Vec3.h"
#include "ui/uiInternal.h"

struct rui_draw_data_t {
  float *vertices;
  size_t verticesCount;
  unsigned int *indices;
  size_t indicesCount;
  uint16_t commandCount;
  size_t vertexCount;
};

enum reh_error_code_e rui_Begin(struct rui_context_t *uiCtx);
enum reh_error_code_e rui_SetupRenderData(GLuint *program, GLuint *VAO, GLuint *VBO, GLuint *EBO);
enum reh_error_code_e rui_RenderRect(struct rui_context_t *uiCtx, float x, float y, const char* label, float w, float h, struct rm_vec3_t color);
enum reh_error_code_e rui_End(struct rui_context_t *uiCtx, GLuint *program, GLuint *VAO, GLuint *VBO, GLuint *EBO, float **projectionMatrixPtr);

enum reh_error_code_e rui_DrawRect(float x, float y, float w, float h, struct rm_vec3_t color, struct rui_draw_data_t *drawData);

enum reh_error_code_e rui_AABBCollisionCheck(struct rui_command_t *A, struct rui_command_t *B, bool *collision);

#endif // RUI_H
