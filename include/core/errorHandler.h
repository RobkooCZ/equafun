#ifndef ERROR_HANDLER_H
#define ERROR_HANDLER_H

// error codes enum
enum ErrorCode {
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

  // Render errors (5xx)
  ERR_GRAPH_SETUP_FAILED = 500,
  ERR_MARKER_SETUP_FAILED = 501,
  ERR_RENDER_INVALID_PARAMS = 502,

  // Generic errors (9xx)
  ERR_UNKNOWN = 999
};

// error context struct
typedef struct errorContext_t {
  enum ErrorCode code;        // Error code of the error
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
    char _msg_buf[256];                                              \
    snprintf(_msg_buf, sizeof(_msg_buf), msg, ##__VA_ARGS__);        \
    setError(code, __FILE__, __LINE__, __func__, _msg_buf, nullptr); \
    return code;                                                     \
  } while(0)

// Add context to existing error without clearing technical details
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
    enum ErrorCode _err = call;                                                         \
    if (_err != ERR_SUCCESS) {                                                          \
      const ErrorContext* _ctx = getLastError();                                        \
      char _new_msg[256];                                                               \
      snprintf(_new_msg, sizeof(_new_msg), msg, ##__VA_ARGS__);                         \
      setError(_err, __FILE__, __LINE__, __func__, _new_msg, _ctx->technicalDetails);   \
      return _err;                                                                      \
    }                                                                                   \
  } while(0)

void setError(enum ErrorCode code, const char* file, int line, const char* fnName, const char* message, const char* technicalInfo);
const ErrorContext* getLastError(void);
void clearError(void);

#endif // ERROR_HANDLER_H
