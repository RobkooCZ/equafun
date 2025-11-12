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

# --- Default build type is Release ---
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
    message(STATUS "Using default CMAKE_BUILD_TYPE. (Release)")
endif()

message(STATUS "CMAKE_BUILD_TYPE: ${CMAKE_BUILD_TYPE}")

# --- Compiler flag for all configurations ---
add_compile_options(-Wall)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g -D_DEBUG_NEABLE -Og -Wextra -Werror")
elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O3 -Wall -funroll-loops")
elseif(CMAKE_BUILD_TYPE STREQUAL "MinSizeRel")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Os")
elseif(CMAKE_BUILD_TYPE STREQUAL "Flags")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wextra -Werror -Wpedantic -Wshadow -Wpointer-arith -Wcast-qual -Wcast-align -Wconversion -Wuninitialized -Wno-unused-function -Wnull-dereference -Wdouble-promotion -Wfloat-equal -Wlogical-op -Wunreachable-code -Wvla -Wmissing-prototypes -Wredundant-decls -Wformat=2 -Wformat-nonliteral -Wformat-security -Wno-missing-field-initializers -Wstrict-aliasing -Wstrict-prototypes -Wunused-variable -Wunused-parameter -Wunused-but-set-variable -Wcast-align -Wno-implicit-fallthrough -Wdeclaration-after-statement -Winline -Wunsafe-loop-optimizations -Wstack-protector -fstack-protector-all -D_FORTIFY_SOURCE=2 -fno-common -fwrapv -fno-strict-aliasing -fno-builtin -ffast-math -funroll-loops -fno-omit-frame-pointer -fstack-check -g -Og -flto -march=native -mtune=native -fno-omit-frame-pointer -fvisibility=hidden -fsanitize=address -fsanitize=undefined -fsanitize=leak -fno-inline -std=c23 -pedantic-errors -fno-common")
else()
  message(FATAL_ERROR "Unknown build type ${CMAKE_BUILD_TYPE}")
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

# --- Packages ---
find_package(Freetype CONFIG REQUIRED)

# If user is on linux
if(UNIX)
  # Locate pkgconfig to check compositor
  find_package(PkgConfig QUIET)

  # pkgconfig not found
  if (NOT PkgConfig_FOUND)
    message(FATAL_ERROR "PkgConfig not found, please install it.")
  endif()

  # check for wayland client package
  pkg_check_modules(WAYLAND REQUIRED wayland-client)

  # user is on wayland
  if(WAYLAND_FOUND)
    message(STATUS "Running under Wayland, checking for system GLFW")

    # attempt to locate system glfw
    pkg_check_modules(GLFW3 QUIET glfw3)

    if(NOT GLFW3_FOUND) # wasnt found
      message(FATAL_ERROR "GLFW with Wayland support not found! Please install GLFW with Wayland support.")
    else() # found
      message(STATUS "System GLFW found")
      include_directories(${GLFW3_INCLUDE_DIRS})
      link_directories(${GLFW3_LIBRARY_DIRS})
    endif()
  else()
    # use vcpkg to install/find glfw3 (for X11)
    find_package(glfw3 CONFIG REQUIRED)
  endif()

  list(APPEND EXTRA_LIBS m)  # math library for Linux
endif()

# --- Link libraries ---
set(EXTRA_LIBS glfw Freetype::Freetype)

# --- Define executable ---
add_executable(equafun ${SRC_FILES})

# --- Specifically for glad, remove some flags as they throw errors I can't fix
set_source_files_properties(libs/src/glad/glad.c PROPERTIES COMPILE_FLAGS "-Wno-pedantic -Wno-sign-conversion")

target_link_libraries(equafun PRIVATE ${EXTRA_LIBS})

# --- Output binary location ---
set_target_properties(equafun PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/build"
    OUTPUT_NAME "equafun"
)

# --- Organize files in IDEs ---
source_group(TREE ${CMAKE_SOURCE_DIR} FILES ${SRC_FILES})

# --- Custom target to copy assets ---
add_custom_target(copy_assets ALL
  COMMAND ${CMAKE_COMMAND} -E rm -rf $<TARGET_FILE_DIR:equafun>/data
  COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/data $<TARGET_FILE_DIR:equafun>/data
)
