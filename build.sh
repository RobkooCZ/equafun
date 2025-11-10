#!/bin/bash

if [ "$1" = "make" ]; then
  make clean
  bear -- make
elif [ "$1" = "cmake" ]; then
  rm -rf build
  cmake -S . -B build
  cmake --build build
else
  echo "Unknown argument: $1"
  echo "Valid arguments:"
  echo "  make - for compilation with make (Unix)"
  echo "  cmake - for compilation with CMake (Unix, Windows)"
fi
