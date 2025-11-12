#ifndef ERROR_HANDLER_H
#define ERROR_HANDLER_H

#include <stdio.h> // for snprintf() and similar, to avoid excess inclusion in every file using this handler

// error codes enum
enum reh_error_code_e {
  ERR_SUCCESS = 0,

  // File I/O errors (1xx)
  ERR_FILE_NOT_FOUND = 100,
  ERR_FILE_READ_FAILED = 101,
  ERR_FILE_WRITE_FAILED = 102,
  ERR_FILE_INVALID_FORMAT = 103,

  // Memory errors (2xx)
  ERR_OUT_OF_MEMORY = 200,
  ERR_ALLOCATION_FAILED = 201,
  ERR_INVALID_POINTER = 202,
  ERR_INVALID_MEMORY_ACCESS = 203,

  // Shader errors (3xx)
  ERR_SHADER_COMPILE_FAILED = 300,
  ERR_SHADER_LINK_FAILED = 301,
  ERR_SHADER_SOURCE_NULL = 302,
  ERR_SHADER_INVALID_TYPE = 303,
  ERR_UNIFORM_NOT_FOUND = 304,

  // OpenGL/Graphics errors (4xx)
  ERR_GLFW_INIT_FAILED = 400,
  ERR_GLAD_INIT_FAILED = 401,
  ERR_WINDOW_CREATE_FAILED = 402,
  ERR_BUFFER_SETUP_FAILED = 403,
  ERR_INVALID_VAO = 404,
  ERR_INVALID_VBO = 405,
  ERR_INVALID_EBO = 406,
  ERR_GLFW_ERR_OCCURED = 407,

  // Render errors (5xx)
  ERR_GRAPH_SETUP_FAILED = 500,
  ERR_MARKER_SETUP_FAILED = 501,
  ERR_RENDER_INVALID_PARAMS = 502,

  // Font/Text errors (6xx)
  ERR_FT_FAILED_TO_INIT = 600,
  ERR_FT_FACE_FAILED_TO_INIT = 601,
  ERR_FT_FACE_UNKNOWN_FILE_FORMAT = 602,
  ERR_FT_FACE_FAILED_TO_SET_FONT_SIZE = 603,
  ERR_FT_FAILED_TO_LOAD_CHAR = 604,

  // Math errors (7xx)
  ERR_DIVISION_BY_ZERO = 700,
  ERR_INVALID_SQRT = 701,
  ERR_OVERFLOW = 702,
  ERR_UNDERFLOW = 703,
  ERR_VALUE_IS_NAN = 704,
  ERR_OUT_OF_BOUNDS = 705,
  ERR_TAN_OUT_OF_DOMAIN = 706,
  ERR_LN_OUT_OF_DOMAIN = 707,
  ERR_LOG_OUT_OF_DOMAIN = 708,

  // Expression evaluator errors (8xx)
  ERR_INPUT_TOKEN_INVALID = 800,
  ERR_INVALID_OPERATOR = 801,
  ERR_UNKNOWN_IDENTIFIER = 802,
  ERR_MISMATCHED_PARENTHESES = 803,
  ERR_INVALID_STACK_STATE = 804,

  // Generic errors (9xx)
  ERR_INVALID_INPUT = 900,
  ERR_UNKNOWN = 999
};

// error context struct
typedef struct reh_error_context_t {
  enum reh_error_code_e code; // Error code of the error
  const char* file;           // File where it happened (__FILE__)
  int line;                   // Line where it happened (__LINE__)
  const char* fnName;         // Name of the function (__func__)
  char message[256];          // Human-readable context message
  char technicalDetails[512]; // Lower level technical info
} ErrorContext;

// macros to remove boilerplate from code

// Set error and return error code
#define SET_ERROR_RETURN(code, msg, ...)                             \
  do {                                                               \
    char _msg_buf[512];                                              \
    snprintf(_msg_buf, sizeof(_msg_buf), msg, ##__VA_ARGS__);        \
    setError(code, __FILE__, __LINE__, __func__, _msg_buf, nullptr); \
    return code;                                                     \
  } while(0)

// Add context to existing error without clearing technical details203
#define ADD_ERROR_CONTEXT_RETURN(code, msg, ...)                                    \
  do {                                                                              \
    const ErrorContext* _ctx = getLastError();                                      \
    char _new_msg[256];                                                             \
    snprintf(_new_msg, sizeof(_new_msg), msg, ##__VA_ARGS__);                       \
    setError(code, __FILE__, __LINE__, __func__, _new_msg, _ctx->technicalDetails); \
    return code;                                                                    \
  } while(0)

// Set error with technical details and return
#define SET_ERROR_TECHNICAL_RETURN(code, msg, technical, ...)          \
  do {                                                                 \
    char _msg_buf[256];                                                \
    snprintf(_msg_buf, sizeof(_msg_buf), msg, ##__VA_ARGS__);          \
    setError(code, __FILE__, __LINE__, __func__, _msg_buf, technical); \
    return code;                                                       \
  } while(0)

// Check error and propagate with additional context
#define CHECK_ERROR_CTX(call, msg, ...)                                                 \
  do {                                                                                  \
    enum reh_error_code_e _err = call;                                                  \
    if (_err != ERR_SUCCESS){                                                           \
      const ErrorContext* _ctx = getLastError();                                        \
      char _new_msg[256];                                                               \
      snprintf(_new_msg, sizeof(_new_msg), msg, ##__VA_ARGS__);                         \
      setError(_err, __FILE__, __LINE__, __func__, _new_msg, _ctx->technicalDetails);   \
      return _err;                                                                      \
    }                                                                                   \
  } while(0)

void setError(enum reh_error_code_e code, const char* file, int line, const char* fnName, const char* message, const char* technicalInfo);
const ErrorContext* getLastError(void);
void clearError(void);

#endif // ERROR_HANDLER_H
