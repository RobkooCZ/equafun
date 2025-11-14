#ifndef FUNCTION_MANAGER_H
#define FUNCTION_MANAGER_H

#include "core/errorHandler.h"
#include "expressionEngine/parser/shuntingYard.h"
#include "math/Vec3.h"

#define MAX_FN_NAME_LEN    1
#define MAX_PARAM_NAME_LEN 1

#define MAX_FUNCTIONS      16

#define WHITE  {1.0f, 1.0f, 1.0f}       // RGB: 255, 255, 255
#define RED    {1.0f, 0.0f, 0.0f}       // RGB: 255, 0, 0
#define ORANGE {1.0f, 0.647f, 0.0f}     // RGB: 255, 165, 0
#define YELLOW {1.0f, 1.0f, 0.0f}       // RGB: 255, 255, 0
#define GREEN  {0.0f, 1.0f, 0.0f}       // RGB: 0, 255, 0
#define BLUE   {0.0f, 0.0f, 1.0f}       // RGB: 0, 0, 255
#define PURPLE {0.5f, 0.0f, 0.5f}       // RGB: 128, 0, 128
#define PINK   {1.0f, 0.41f, 0.71f}     // RGB: 255, 105, 180
#define GRAY   {0.5f, 0.5f, 0.5f}       // RGB: 128, 128, 128

extern struct rm_vec3_t functionColorArray[];
extern const int functionColorArrayLength;

struct ree_function_t {
  char name[MAX_FN_NAME_LEN + 1];         // f, g, h, etc
  char parameter[MAX_PARAM_NAME_LEN + 1]; // x for example

  struct ree_token_t *tokens;             // tokens from the lexer without 'f(x) ='
  int tokenCount;                         // token count
  int tokenCapacity;

  struct ree_output_token_t *rpn;         // rpn of the function definition
  int rpnCount;                           // rpn token count

  bool isVisible;                         // flag to determine whether the function is to be rendered
  struct rm_vec3_t color;                 // color of the function
};

struct ree_function_manager_t {
  struct ree_function_t functions[MAX_FUNCTIONS];
  int functionCount;
};

enum reh_error_code_e ree_initFunctionManager(struct ree_function_manager_t *manager);
enum reh_error_code_e ree_addFunction(struct ree_function_manager_t *manager, char *definition, struct rm_vec3_t *functionColor);
enum reh_error_code_e ree_removeFunction(struct ree_function_manager_t *manager, const char* name);
int ree_getFunction(struct ree_function_manager_t *manager, const char *name, struct ree_function_t *function);

#endif//FUNCTION_MANAGER_H
