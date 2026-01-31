/*
  rui - Robkoo's UI
*/

#ifndef RUI_INPUT_FIELD_H
#define RUI_INPUT_FIELD_H

#include "core/appContext.h"
#include "glad/glad.h"
#include <GLFW/glfw3.h>

#include "core/errorHandler.h"
#include "textRenderer/text.h"
#include "ui/uiInternal.h"
#include <stddef.h>

#define INPUT_FIELD_CHAR_CAP 64

struct rui_input_cursor_data_t {
  float cursorXPos; // x of the cursor pos, y is irrelevant as the cursor will span from top of the input field (taking into account padding) to the bottom
  unsigned int cursorCharPos; // for example, if its 4, the cursor is after the 4th character in the input field. Used for moving around in the string, deleting and appending correctly
};

struct rui_input_field_data_t {
  _Bool isInitialized;

  float textScale;
  float inputX;
  float inputY;
  struct rui_input_cursor_data_t *cursor;

  char* inputText; // what's currently in the input field
  size_t charCount;
};

struct rui_global_input_field_data_t {
  bool isAnyPressed; // true if any input field is pressed
  int activeId; // id of the active input field (user has pressed on this one)
};

struct rui_input_fields_t {
  int id; // identifier
  struct rui_input_field_data_t data;
};

void charCallback(GLFWwindow *window, unsigned int codepoint);

void rui_InputFieldOnClick(struct rui_command_input_field_t *command);
enum reh_error_code_e rui_InputFieldInput(GLFWwindow *window);
enum reh_error_code_e rui_RenderInputFieldText(struct rui_command_input_field_t *command, struct ra_app_context_t *ctx, struct rtr_character_t* characters, const char* fieldData);

// not thread safe
extern struct rui_global_input_field_data_t g_inputFieldData;
extern struct rui_input_fields_t g_inputFields[RUI_MAX_COMMANDS];

/*
  onClick will render the input field data indepedently of the ui renderer, as it only has to be updated when the user has clicked on the specific input field, and thus re-rendered. However, the input field data have to be stored elsewhere (probably in the input command structure), otherwise we wouldn't have access to the previous data after the user clicks again on the input field.
    - could be changed that it gets stored in a file somewhere, but that would hamper performance, as file I/O takes longer than accessing system memory. HOWEVER, if there was ever a concern over huge RAM usage, this could be a way to lower the usage as the structure wouldn't be stored in system memory, but on the drive. That would mean I'd have to implement a file I/O controller to save, parse and extract the data.

  onClick can render indepedently due to the fact, that the ui will be static, and thus when the input field is rendered, it will ALWAYS stay there regardless of anything. this is subject to change. the input text rendering function will be separate from the onClick handler, to separate concerns and also future-proof the file as if i ever implement resizing of the ui, according to, for example, the window's dimensions changing, there will be a possibility of calling the render function on the changed input fields separately and thus also change how the text looks.
*/

/*
  rough order of implementation:
    1. check that the onclick would work successfully and save data.
    3. make onclick take user input as characters to input, so a-z, A-Z, 0-9, (), =
      - not yet render the data, just make it save into the data struct
      - pressing ESC makes it go out of onClick, and thus no more input will be taken for the input field
    4. implement cursor moving (right arrow key, left arrow key), also no rendering yet but it correctly updates the data struct with the cursor data
    5. text rendering + cursor rendering
      - cursor is a line with points as follows: TOP[cursorX; inputFieldY + padding], BOTTOM[cursorX; inputFieldY + height - padding]
      - calculate text scale as follows: scale = h/INITIAL_TEXT_SIZE where h is the height of the input field. account for padding.
      - render the text starting at inputFieldX;inputFieldY. that data should be in the input field data struct.
      - if possible, re-render the input field only on input key press (so anything that would count as input for the field) or on left/right arrow key press (cursor moving)
        - i reckon that would be more advanced becuase i'd have to use a texture to render the input field onto and then every frame render the texture. that would be less computationally expensive rather than every frame rendering the entire input field over and over again. on change it would write new data to the texture and texture would render that.
    6. if the input function is too long, and the text would overflow the input field, there needs to be a horizontal scrolling added, where it also renders "x) = x^2" (so it would not show the beginning of the function that wouldn't fit, and would instead start somewhere where the rest of the string would fit)
      - also includes implementing the scrolling with the cursor moving, so the user can access the entire string regardless of how big it is
more advanced stuff:
- ctrl+c, ctrl+v for copying and pasting
  - that would include highlighting
- shift + right/left arrow to highlight str using the text
>> both of the above include adding new fields to the cursor structure to hold the highlighted data <<


*/

#endif
