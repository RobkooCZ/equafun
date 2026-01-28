#include "ui/inputField.h"
#include "core/errorHandler.h"
#include "core/logger.h"
#include "core/window.h"
#include "math/Vec3.h"
#include "textRenderer/text.h"
#include "ui/ui.h"
#include "ui/uiInternal.h"
#include "utils/utilities.h"
#include <GLFW/glfw3.h>
#include <string.h>

struct rui_global_input_field_data_t g_inputFieldData = {false, 0};
struct rui_input_fields_t g_inputFields[RUI_MAX_COMMANDS];

void rui_InputFieldOnClick(struct rui_command_input_field_t *command){
  g_inputFieldData.isAnyPressed = true;
  command->borderColor = (struct rm_vec3_t){1.0f, 1.0f, 0.0f};

  // get the contents of the field
  char fieldContents[INPUT_FIELD_CHAR_CAP];
  for (int i = 0; i < uiCtx.commandCount; ++i){
    // primitive
    if (uiCtx.commands[i].type == RUI_INPUT_FIELD && uiCtx.commands[i].id == g_inputFieldData.activeId && g_inputFields[i].data.charCount <= INPUT_FIELD_CHAR_CAP){
      strcpy(fieldContents, g_inputFields[i].data.inputText);
    }
  }

  //rl_LogMsg(RL_DEBUG, "contents: %s", fieldContents);
}

enum reh_error_code_e rui_InputFieldInput(GLFWwindow *window){
    if (glfwGetKey(window, GLFW_KEY_BACKSPACE) == GLFW_PRESS){
        for (int i = 0; i < uiCtx.commandCount; ++i){
          if (uiCtx.commands[i].type == RUI_INPUT_FIELD && uiCtx.commands[i].id == g_inputFieldData.activeId && g_inputFields[i].data.charCount > 0){
            g_inputFields[i].data.inputText[--g_inputFields[i].data.charCount]= '\0';
              rl_LogMsg(RL_DEBUG, "Input field data\n\tID: %d\n\tInput Text: %s\n", uiCtx.commands[i].id, g_inputFields[i].data.inputText);
          }
        }
        rgu_msleep(50);
      }
    else {
      for (int keycode = GLFW_KEY_A; keycode <= GLFW_KEY_Z; keycode++){
        if (glfwGetKey(window, keycode) == GLFW_PRESS){
          const char* key = glfwGetKeyName(keycode, 0);
          // find what input field to add the data to
          for (int i = 0; i < uiCtx.commandCount; ++i){
            // primitive
            if (uiCtx.commands[i].type == RUI_INPUT_FIELD && uiCtx.commands[i].id == g_inputFieldData.activeId && g_inputFields[i].data.charCount <= INPUT_FIELD_CHAR_CAP){
              strcat(g_inputFields[i].data.inputText, key);
              g_inputFields[i].data.charCount++;
              rl_LogMsg(RL_DEBUG, "Input field data\n\tID: %d\n\tInput Text: %s\n\tKey: %s", uiCtx.commands[i].id, g_inputFields[i].data.inputText, key);
            }
          }
          rgu_msleep(150);
        }
      }
    }
  return ERR_SUCCESS;
}


enum reh_error_code_e rui_RenderInputFieldText(struct rui_command_input_field_t *command, struct ra_app_context_t *ctx, struct rtr_character_t* characters, const char* fieldData){
  float topYGL = windowHeight - command->y - command->innerPadding - command->borderSize * 3.0f; // random magic number in place before i make a calculation to always center the text in the input field

  rtr_RenderText(ctx->textProgram, ctx->textVAO, ctx->textVBO, fieldData, characters, command->x + command->borderSize * 3.0f, topYGL, 1.0f, (struct rm_vec3_t){1.0f, 1.0f, 1.0f});

  // rl_LogMsg(RL_DEBUG, "Attempting to render field text. X: %f, Y: %f\n\ttPr:%d tVAO:%d tVBO:%d", command->x, command->y, ctx->textProgram, ctx->textVAO, ctx->textVBO);

  return ERR_SUCCESS;
}
