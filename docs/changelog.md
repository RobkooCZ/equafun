# Changelog

## Alpha v0.0.6.2.1

### Fixed
- precision issue causing the pi value detector to not correctly detect higher multiples of pi (such as 4pi and more)

## Alpha v0.0.6.2

### Added
- debounce to input
- custom function for sleeping in ms (both operating systems)
- pressing 'P' toggles the x-axis labels' between integer values (1, 2, 3, ...) to pi values (pi/2, pi, 3pi/2, ...)

## Alpha v0.0.6.1

### Added
- decimal support

## Alpha v0.0.6

### Added
- a lexer
    - splits the provided expression into tokens and value 'pairs'
    - ignores whitespaces
    - support for numbers, identifiers, equals sign, functions (predefined), unary and binary operators (including factorial)
- a parser
    - takes the 'lexed' tokens and parses them using the Shunting Yard algorithm
    - supports parentheses
    - distinguishes between unary and binary minus (and plus) operators
    - support for factorial operator with proper precedence
    - is able to recognize one-letter identifiers to then be substituted for a value in the evaluator
    - spits out RPN (reverse polish notation) which is handled in the evaluator
    - support for implicit multiplication
- an evaluator
    - supports variables
    - solves for y when given an expression and variables data
    - correctly evaluates functions, operators (including unary) and numbers
- function to parse the a function including the definition and save the data into the function struct
- a global utilities file with
    - a function to search for a string in an array of strings
    - a function to trim the leading whitespace in a string
- on window resize:
    - projection matrices get recalculated
    - graph VBO gets updated
    - window contents get redrawn
- function rendering
    - is able to handle non-continuous functions
    - can render multiple functions at the same time
    - step is for now hardcoded
- function manager
    - holds function structures which hold all function data
    - added functions to add, remove, init and lookup
- CMake and `vcpkg.json` integration for standardized building
- a `build.cmd` script to streamline compilation on Windows
- brief comments to all functions
- comments to each header file explaining the functions' id

### Changed
- updated makefile to use `find ..` instead of `wildcard` to match all `.c` files inside the project rather than specific nested structures (`src/**/*.c`)
- `_msg_buf` in `SET_ERROR_RETURN()` macro size to 512
- graph, marker and text rendering to use world coordinates instead of NDC coordinates
- the graph now stays square (as in, if you extended the markers, everything would look like squares) instead of rectangles and similar
- split up main into 3 functions - ra_AppInit, ra_AppRenderFrame and ra_AppShutdown
- cleaned up repetitive error checking code in ra_AppInit
- centralized cleanup after a failure
- heavily improved both the `build.sh` script
    - argument to choose between CMake and make
    - argument to toggle generating `compile_commands.json` or not
    - argument to choose a preset to compile with (for both CMake and make)
- undefined points don't get connected anymore
- all function signatures to meet standards defined in `standards.md`

### Fixed
- minor rendering issues
- code to be stricter to allow compilation with the 'Flags' preset
- null-pointer bug in `reh_SetError()`
- a bug where fread() and ftell() returned with different byte values on Windows
- error handler not printing all the error messages

## Alpha v0.0.5
### Added
- FreeType text rendering system
    - character atlas loading (128 ASCII characters)
    - OpenGL texture generation for each glyph
    - dynamic text rendering with VAO/VBO management
- axis labeling system
    - origin (0,0) label
    - X-axis labels (positive and negative)
    - Y-axis labels (positive and negative)
    - NDC to pixel coordinate conversion helpers
    - text width/height calculation for proper alignment
    - value formatting (e.g., "1.00", "2.50")
- math utilities
    - Vec2 structure and basic operations
    - Vec3 basic structures
    - Mat4 orthographic projection
    - type conversion function (`rm_Mat4ValuePtr()`)
- shader utilities
    - `rsu_GluSetMat4()` for setting matrix uniforms

### Changed
- graph markers now use dynamic vertex allocation
- marker rendering calculates visible bounds each frame
- text rendering uses orthographic projection matrix
- `enum ErrorCode` to `enum reh_error_code_e`  and `struct ErrorContext_t` to `struct reh_error_context_t`  across the codebase to match the standards
- build system updated to include FreeType library linkage
- render loop now includes text rendering pass for axis labels
- makefile now supports recompiling if a header file was changed

### Fixed
- text rendering coordinate system matches graph coordinate system

## Alpha v0.0.4

### Added
- comprehensive two-layer error handling system
    - `struct reh_error_context_t` struct to store error details (code, file, line, function name, message, technical details)
    - global error context tracking via `reh_GetLastError()`, `reh_SetError()`, and `reh_ClearError()` functions
    - error code enumeration (`enum reh_error_code_e`) with categorized error codes (File I/O, Memory, Shader, OpenGL/Graphics, Render, Generic)
- error handling macros to reduce boilerplate
    - `SET_ERROR_RETURN()` - set error with message and return error code
    - `SET_ERROR_TECHNICAL_RETURN()` - set error with technical details and return
    - `CHECK_ERROR_CTX()` - check error code and propagate with additional context
- logger integration with error system
    - `rl_LogError()` - log error context with technical details
    - `rl_LogLastError()` - log the most recent error from global context
- thorough OpenGL error checking using `glGetError()` after all OpenGL calls in render utilities

### Changed
- all (error-possible) function signatures now return `enum reh_error_code_e`
- error handling architecture follows two-layer pattern
    - utilities: silent error detection, stores technical details in `struct reh_error_context_t`, returns error codes
    - setup/main: checks error codes, adds high-level context, logs errors with `rl_LogLastError()`
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
    - if the user wishes to not have `RL_DEBUG` messages printed, they can comment/remove the macro `_DEBUG_ENABLE` located in `include/core/logger.h`
- simple input function that checks for the `ESC` key to be pressed to close the window

### Changed
- minor changes in `README.md`
- changed `[f]printf()` messages to the custom log function `rl_LogMsg()`
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
