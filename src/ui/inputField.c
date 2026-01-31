#include "ui/inputField.h"
#include "core/errorHandler.h"
#include "core/logger.h"
#include "core/window.h"
#include "expressionEngine/functionManager.h"
#include "math/Vec3.h"
#include "textRenderer/text.h"
#include "ui/ui.h"
#include "ui/uiInternal.h"
#include "utils/utilities.h"
#include <GLFW/glfw3.h>
#include <string.h>

struct rui_global_input_field_data_t g_inputFieldData = {false, 0};
struct rui_input_fields_t g_inputFields[RUI_MAX_COMMANDS];

void rui_inputFieldCharCallback(GLFWwindow *window, unsigned int codepoint){
  // range of unicode characters we wanna accept for the input:
    // U+0020 - <SPACE>
    //        -
    // U+007A - lowercase z

  if (codepoint < 0x20 || codepoint > 0x7A) return;

  for (int i = 0; i < g_uiCtx.commandCount; ++i){
    if (g_uiCtx.commands[i].type == RUI_INPUT_FIELD && g_uiCtx.commands[i].id == g_inputFieldData.activeId){
      g_inputFields[i].data.inputText[g_inputFields[i].data.charCount++] = (char)codepoint;
        rl_LogMsg(RL_DEBUG, "Input field data\n\tID: %d\n\tInput Text: %s\n\tKey: %c", g_uiCtx.commands[i].id, g_inputFields[i].data.inputText, codepoint);
    }
  }
}

void rui_InputFieldOnClick(struct rui_command_input_field_t *command){
  g_inputFieldData.isAnyPressed = true;
  command->borderColor = (struct rm_vec3_t){1.0f, 1.0f, 0.0f};

  // get the contents of the field
  char fieldContents[INPUT_FIELD_CHAR_CAP];
  for (int i = 0; i < g_uiCtx.commandCount; ++i){
    // primitive
    if (g_uiCtx.commands[i].type == RUI_INPUT_FIELD && g_uiCtx.commands[i].id == g_inputFieldData.activeId && g_inputFields[i].data.charCount <= INPUT_FIELD_CHAR_CAP){
      strcpy(fieldContents, g_inputFields[i].data.inputText);
    }
  }

  //rl_LogMsg(RL_DEBUG, "contents: %s", fieldContents);
}

enum reh_error_code_e rui_InputFieldInput(GLFWwindow *window){
  glfwSetCharCallback(window, rui_inputFieldCharCallback);
  if (glfwGetKey(window, GLFW_KEY_BACKSPACE) == GLFW_PRESS){ // backspace
      for (int i = 0; i < g_uiCtx.commandCount; ++i){
        if (g_uiCtx.commands[i].type == RUI_INPUT_FIELD && g_uiCtx.commands[i].id == g_inputFieldData.activeId && g_inputFields[i].data.charCount > 0){
          g_inputFields[i].data.inputText[--g_inputFields[i].data.charCount] = '\0';
            rl_LogMsg(RL_DEBUG, "Input field data\n\tID: %d\n\tInput Text: %s\n\tKey: <BACKSPACE>", g_uiCtx.commands[i].id, g_inputFields[i].data.inputText);
        }
      }
      rgu_msleep(50);
  }
  else if (glfwGetKey(window, GLFW_KEY_ENTER) == GLFW_PRESS){
    for (int i = 0; i < g_uiCtx.commandCount; ++i){
      if (g_uiCtx.commands[i].type == RUI_INPUT_FIELD && g_uiCtx.commands[i].id == g_inputFieldData.activeId && g_inputFields[i].data.charCount > 0){
        char* fnDef = g_inputFields[i].data.inputText;
        ree_AddFunction(&g_functions, fnDef, &g_functionColorArray[g_colorIterator]);

        INCREMENT_COLOR_ITERATOR;

        // clear the input field's data
        memset(g_inputFields[i].data.inputText, 0, strlen(g_inputFields[i].data.inputText));
        g_inputFields[i].data.charCount = 0;
      }
    }
  }
  return ERR_SUCCESS;
}


enum reh_error_code_e rui_RenderInputFieldText(struct rui_command_input_field_t *command, struct ra_app_context_t *ctx, struct rtr_character_t* characters, const char* fieldData){
  struct rtr_character_t glyphA = characters[96]; // lowercase a

  float scale = 1.0f;

  float glyphHeight = glyphA.size.y * scale;

  float topYGL = g_windowHeight - command->y - command->innerPadding - command->borderSize;

  // remaining space is the space between the bottom border of the text until the border of the input field at the bottom
  float remainingSpace = (command->h - 2.0f * command->borderSize) - glyphHeight;

  topYGL -= remainingSpace / 2.0f;

  rtr_RenderText(ctx->textProgram, ctx->textVAO, ctx->textVBO, fieldData, characters, command->x + command->borderSize, topYGL, 1.0f, (struct rm_vec3_t){1.0f, 1.0f, 1.0f});
  // rl_LogMsg(RL_DEBUG, "Attempting to render field text. X: %f, Y: %f\n\ttPr:%d tVAO:%d tVBO:%d", command->x, command->y, ctx->textProgram, ctx->textVAO, ctx->textVBO);

  return ERR_SUCCESS;
}
