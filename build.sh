#!/bin/bash

if [[ "$1" = "make" && "$2" = "-b" ]]; then
  make clean
  bear -- make
elif [[ "$1" = "make" ]]; then
  make clean
  make
elif [[ "$1" = "cmake" && "$2" = "-b" ]]; then
  rm -rf build
  cmake -S . -B build -DCMAKE_TOOLCHAIN_FILE=~/vcpkg/scripts/buildsystems/vcpkg.cmake
  bear -- cmake --build build
elif [[ "$1" = "cmake" ]]; then
  rm -rf build
  cmake -S . -B build -DCMAKE_TOOLCHAIN_FILE=~/vcpkg/scripts/buildsystems/vcpkg.cmake
  cmake --build build
else
  echo "Unknown argument: $1 $2"
  echo "Valid arguments:"
  echo "  'make'     - for compilation with make (Unix)"
  echo "  'make -b'  - for compilation with bear + make to generate compile_commands.json (Unix)"
  echo "  'cmake'    - for compilation with CMake (Unix, Windows)"
  echo "  'cmake -b' - for compilation with bear + CMake to generate compile_commands.json (Unix, Windows)"
fi
