/*
  rui - Robkoo's UI
*/

#ifndef   RUI_RECT_ON_EVENT
#define   RUI_RECT_ON_EVENT

#include "ui/uiInternal.h"

#define RECT_ON_CLICK_COLOR(r,g,b,colorName) \
  void rui_RectOnClick##colorName(struct rui_command_rect_t *rect){  \
    if (!rect) return; \
    rect->color = (struct rm_vec3_t){ (r), (g), (b) }; \
  } \

#define RECT_ON_HOVER_COLOR(r,g,b,colorName) \
  void rui_RectOnHover##colorName(struct rui_command_rect_t *rect){  \
    if (!rect) return; \
    rect->color = (struct rm_vec3_t){ (r), (g), (b) }; \
  } \

RECT_ON_CLICK_COLOR(1.0f, 0.0f, 0.0f, Red);
RECT_ON_CLICK_COLOR(0.0f, 1.0f, 0.0f, Green);
RECT_ON_CLICK_COLOR(0.0f, 0.0f, 1.0f, Blue);
RECT_ON_CLICK_COLOR(0.2f, 0.2f, 0.2f, Gray);

RECT_ON_HOVER_COLOR(1.0f, 0.0f, 0.0f, Red);
RECT_ON_HOVER_COLOR(0.0f, 1.0f, 0.0f, Green);
RECT_ON_HOVER_COLOR(0.0f, 0.0f, 1.0f, Blue);
RECT_ON_HOVER_COLOR(0.2f, 0.2f, 0.2f, Gray);

#endif // RUI_RECT_ON_EVENT
