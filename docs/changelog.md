# Changelog

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
