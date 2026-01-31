/**
  ree - Robkoo's Expression Engine
*/

#ifndef FUNCTION_MANAGER_H
#define FUNCTION_MANAGER_H

#include "core/errorHandler.h"
#include "expressionEngine/parser/shuntingYard.h"
#include "math/Vec3.h"

#define MAX_FN_NAME_LEN    1
#define MAX_PARAM_NAME_LEN 1

#define REE_MAX_FUNCTIONS         16
#define REE_MAX_DEFINITION_LENGTH 255

#define WHITE  {1.0f, 1.0f, 1.0f}       // RGB: 255, 255, 255
#define RED    {1.0f, 0.0f, 0.0f}       // RGB: 255, 0, 0
#define ORANGE {1.0f, 0.647f, 0.0f}     // RGB: 255, 165, 0
#define YELLOW {1.0f, 1.0f, 0.0f}       // RGB: 255, 255, 0
#define GREEN  {0.0f, 1.0f, 0.0f}       // RGB: 0, 255, 0
#define BLUE   {0.0f, 0.0f, 1.0f}       // RGB: 0, 0, 255
#define PURPLE {0.5f, 0.0f, 0.5f}       // RGB: 128, 0, 128
#define PINK   {1.0f, 0.41f, 0.71f}     // RGB: 255, 105, 180
#define GRAY   {0.5f, 0.5f, 0.5f}       // RGB: 128, 128, 128

#define INCREMENT_COLOR_ITERATOR (g_colorIterator + 1 > g_functionColorArrayLength - 1) ? g_colorIterator = 0 : g_colorIterator++

extern struct rm_vec3_t g_functionColorArray[];
extern const int g_functionColorArrayLength;

extern struct ree_function_manager_t g_functions;
extern int g_colorIterator;

struct ree_function_t {
  char definition[REE_MAX_DEFINITION_LENGTH + 1];   /**< Function definition inputted by the user */
  char name[MAX_FN_NAME_LEN + 1];                   /**< Function name, e.g., f, g, h */
  char parameter[MAX_PARAM_NAME_LEN + 1];           /**< Parameter name, e.g., 'x' */

  struct ree_token_t *tokens;                       /**< Tokens from the lexer without 'f(x) =' */
  int tokenCount;                                   /**< Token count */
  int tokenCapacity;                                /**< Token capacity */

  struct ree_output_token_t *rpn;                   /**< RPN of the function definition */
  int rpnCount;                                     /**< RPN token count */
  bool isVisible;                                   /**< Flag to determine whether the function is to be rendered */
  struct rm_vec3_t color;                           /**< Color of the function */
};

struct ree_function_manager_t {
  struct ree_function_t functions[REE_MAX_FUNCTIONS];  /**< Array of functions managed */
  int functionCount;                                   /**< Current number of functions */
};

/**
  @brief Initializes the function manager
*/
enum reh_error_code_e ree_InitFunctionManager(struct ree_function_manager_t *manager);

/**
  @brief Adds a function to the function manager
*/
enum reh_error_code_e ree_AddFunction(struct ree_function_manager_t *manager, char *definition, struct rm_vec3_t *functionColor);

/**
  @brief Removes a function from the function manager
*/
enum reh_error_code_e ree_RemoveFunction(struct ree_function_manager_t *manager, const char* name);

/**
  @brief Retrieves a function from the function manager based on name
*/
int ree_GetFunction(struct ree_function_manager_t *manager, const char *name, struct ree_function_t *function);

/**
  @brief Checks if a function is in the function manager
*/
bool ree_IsFunctionInManager(struct ree_function_manager_t *manager, const char* name);

#endif//FUNCTION_MANAGER_H
