cmake_minimum_required(VERSION 3.16)
project(equafun C)

# --- Language settings ---
set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

# --- Default build type ---
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

# --- Compiler flags ---
if(MSVC)
    add_compile_options(/W4 /WX)
else()
    add_compile_options(-Wall -Werror -O2 -g)
endif()

# --- Source directories ---
set(SRC_DIR "${CMAKE_SOURCE_DIR}/src")
set(LIBS_SRC_DIR "${CMAKE_SOURCE_DIR}/libs/src")

# --- Gather sources ---
file(GLOB_RECURSE SRC_FILES
    "${SRC_DIR}/*.c"
    "${LIBS_SRC_DIR}/*.c"
)

# --- Include paths for project ---
include_directories(
    "${CMAKE_SOURCE_DIR}/include"
    "${CMAKE_SOURCE_DIR}/libs/include"
)

# --- FREETYPE detection ---
find_package(PkgConfig QUIET)
if(PkgConfig_FOUND)
    pkg_check_modules(FREETYPE2 freetype2)
endif()

if(FREETYPE2_FOUND)
    message(STATUS "Found Freetype via pkg-config")
    include_directories(${FREETYPE2_INCLUDE_DIRS})
    link_directories(${FREETYPE2_LIBRARY_DIRS})
    set(EXTRA_LIBS ${EXTRA_LIBS} ${FREETYPE2_LIBRARIES})
else()
    find_package(Freetype REQUIRED)
    if(Freetype_FOUND)
        message(STATUS "Found Freetype via CMake")
        include_directories(${Freetype_INCLUDE_DIRS})
        set(EXTRA_LIBS ${EXTRA_LIBS} ${Freetype_LIBRARIES})
    endif()
endif()

# --- GLFW detection ---
find_package(glfw3 QUIET)
if(glfw3_FOUND)
    set(EXTRA_LIBS ${EXTRA_LIBS} glfw)
else()
    if(UNIX)
        find_package(OpenGL REQUIRED)
        set(EXTRA_LIBS ${EXTRA_LIBS} glfw GL X11 pthread Xrandr Xi dl m)
    elseif(WIN32)
        set(EXTRA_LIBS ${EXTRA_LIBS} glfw3 opengl32)
    endif()
endif()

# --- Define executable ---
add_executable(equafun ${SRC_FILES})

# --- Link everything ---
if(UNIX)
    set(EXTRA_LIBS ${EXTRA_LIBS} m)  # link libm explicitly
endif()

# --- Link everything ---
target_link_libraries(equafun PRIVATE ${EXTRA_LIBS})

# --- Output binary location (matches Makefile: build/equafun) ---
set_target_properties(equafun PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}"
    OUTPUT_NAME "equafun"
)

# --- Optional: organize files in IDEs ---
source_group(TREE ${CMAKE_SOURCE_DIR} FILES ${SRC_FILES})

