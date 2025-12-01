/*
  rui - Robkoo's UI
*/

#ifndef   RUI_INTERNAL_H
#define   RUI_INTERNAL_H

#include "math/Vec3.h"
#include <stdint.h>

#define RUI_MAX_COMMANDS 1024

enum rui_command_type_e {
  RUI_RECT,
  RUI_TEXT
};

struct rui_command_rect_t {
  float x, y, w, h; // x, y = top left of the drawn object
  struct rm_vec3_t color;
};

struct rui_command_t {
  enum rui_command_type_e type;
  int id;
  union {
    struct rui_command_rect_t rect;
    // eventually text will go here too
  };
};

struct rui_context_t {
  struct rui_command_t commands[RUI_MAX_COMMANDS];
  uint16_t commandCount;
};

 struct rui_state_t {
  double mouseX;
  double mouseY;
  int mouseDown;

  int hotItem;
  int activeItem;
};

// not thread-safe
extern struct rui_state_t ruiState;

#endif // RUI_INTERNAL_H
