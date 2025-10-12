# Changelog

## Alpha v0.0.4

### Added
- comprehensive two-layer error handling system
    - `ErrorContext` struct to store error details (code, file, line, function name, message, technical details)
    - global error context tracking via `getLastError()`, `setError()`, and `clearError()` functions
    - error code enumeration (`enum ErrorCode`) with categorized error codes (File I/O, Memory, Shader, OpenGL/Graphics, Render, Generic)
- error handling macros to reduce boilerplate
    - `SET_ERROR_RETURN()` - set error with message and return error code
    - `SET_ERROR_TECHNICAL_RETURN()` - set error with technical details and return
    - `CHECK_ERROR_CTX()` - check error code and propagate with additional context
- logger integration with error system
    - `logError()` - log error context with technical details
    - `logLastError()` - log the most recent error from global context
- thorough OpenGL error checking using `glGetError()` after all OpenGL calls in render utilities

### Changed
- all function signatures now return `enum ErrorCode`
    - shader utilities: `loadShaderSource()`, `compileShader()`, `linkShaders()`
    - window initialization: `initGLFW()`, `initWindow()`
    - render utilities: `setupRenderData()`, `setupEBO()`
    - graph rendering: `setupGraph()`, `setupMarkers()`, `renderGraph()`, `renderMarkers()`
- error handling architecture follows two-layer pattern
    - utilities: silent error detection, stores technical details in `ErrorContext`, returns error codes
    - setup/main: checks error codes, adds high-level context, logs errors with `logLastError()`
- main function now checks all initialization and setup calls for errors and handles graceful shutdown
- all utility functions now validate input parameters (NULL checks, size checks) before operations
- `.gitattributes` now explicitly tells linguist that all .h and .c files are C,`.md` files are documentation and shaders are GLSL

### Fixed
- memory leaks in shader loading (shader sources now properly freed after use)
- missing error handling for OpenGL buffer operations
- missing NULL pointer checks throughout codebase
- improved error messages with full context chain (technical details -> function context -> high-level failure)

## Alpha v0.0.3
### Added
- basic graph render
- markers to the graph (unmarked)
- utility functions for shaders and rendering
- vertex and fragment shader

## Alpha v0.0.2
### Added
- simple logger
    - prints the provided text (with formatting) to the console using one of 5 severity levels
    - if the user wishes to not have `DEBUG` messages printed, they can comment/remove the macro `_DEBUG_ENABLE` located in `include/core/logger.h`
- simple input function that checks for the `ESC` key to be pressed to close the window

### Changed
- minor changes in `README.md`
- changed `[f]printf()` messages to the custom log function `logMsg()`
- changed OpenGL version to 3.3 for compatibility

### Fixed
- added bg color render to the render loop to prevent the occasional segmentation fault of the `glfwSwapBuffers()` function (due to not rendering anything)

## Alpha v0.0.1
### Added
- file structure
- makefile
- bash script to build the project
- basic OpenGL code to open a window
    - can be resized
