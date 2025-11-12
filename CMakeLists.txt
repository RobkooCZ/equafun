cmake_minimum_required(VERSION 3.16)

# --- VCPKG support ---
# Tell CMake to use vcpkg toolchain if available
if(NOT DEFINED CMAKE_TOOLCHAIN_FILE)
    if(DEFINED ENV{VCPKG_ROOT})
        set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake" CACHE STRING "")
    endif()
endif()

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

# --- Include paths ---
include_directories(
    "${CMAKE_SOURCE_DIR}/include"
    "${CMAKE_SOURCE_DIR}/libs/include"
)

# --- Find Freetype (vcpkg or system) ---
find_package(Freetype CONFIG REQUIRED)

# --- Prefer system GLFW , fallback to vcpkg ---
find_package(PkgConfig QUIET)

# First, try system pkg-config for GLFW
if (PkgConfig_FOUND)
    pkg_check_modules(GLFW3 QUIET glfw3)
endif()

if (GLFW3_FOUND)
    message(STATUS "Found system GLFW via pkg-config (${GLFW3_VERSION})")
    include_directories(${GLFW3_INCLUDE_DIRS})
    link_directories(${GLFW3_LIBRARY_DIRS})
    set(GLFW_LIBRARIES ${GLFW3_LIBRARIES})
else()
    message(STATUS "System GLFW not found, falling back to vcpkg/Config mode")
    find_package(glfw3 CONFIG REQUIRED)
    set(GLFW_LIBRARIES glfw)
endif()

# --- Link libraries ---
set(EXTRA_LIBS glfw Freetype::Freetype)

if(UNIX)
    list(APPEND EXTRA_LIBS m)  # math library for Linux
endif()

# --- Define executable ---
add_executable(equafun ${SRC_FILES})
target_link_libraries(equafun PRIVATE ${EXTRA_LIBS})

# --- Output binary location (matches Makefile: build/equafun) ---
set_target_properties(equafun PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/build"
    OUTPUT_NAME "equafun"
)

# --- Organize files in IDEs ---
source_group(TREE ${CMAKE_SOURCE_DIR} FILES ${SRC_FILES})

add_custom_target(copy_assets ALL
  COMMAND ${CMAKE_COMMAND} -E rm -rf $<TARGET_FILE_DIR:equafun>/data
  COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/data $<TARGET_FILE_DIR:equafun>/data
)
