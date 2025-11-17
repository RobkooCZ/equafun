#!/bin/bash

# Default values
BEAR=false
BUILD_TYPE="Release"
UTILITY="CMake"

# Parse command-line options
while getopts "bt:u:h" opt; do
  case "$opt" in
    b)
      BEAR=true
      ;;
    t)
      if [[ ! "$OPTARG" =~ ^(Debug|Release|MinSizeRel|Flags)$ ]]; then
        echo "Invalid build type provided: $OPTARG"
        echo "Check './build.sh -h' for valid build types."
        exit 1
      else
        BUILD_TYPE=$OPTARG
      fi
      ;;
    u)
      if [[ ! "$OPTARG" =~ ^(make|cmake)$ ]]; then
        echo "Invalid utility provided: $OPTARG"
        echo "Check './build.sh -h' for valid compilation utilities."
        exit 1
      else
        UTILITY=$OPTARG
      fi
      ;;
    h)
      echo "Equafun project build script (Unix)"
      echo "Script version: 0.0.1"
      echo "Latest update: Alpha v0.0.6"
      echo "Options:"
      echo "  -h             | display this help"
      echo "  -b             | compile and generate compile_commands.json"
      echo "  -t [buildType] | provide a build type to the compile utility"
      echo "    Valid options are: Debug, Release, MinSizeRel, Flags"
      echo "  -u [utility]   | what utility you want to use"
      echo "    Valid options are: make, cmake"
      echo "Example use case: './build.sh -u \"Make\" -b -t \"Debug\"'"
      echo "Note: without any arguments, the script defaults to $UTILITY compilation with $BUILD_TYPE as the build type."
      exit 1
      ;;
    \?)
      echo "Invalid option: -$OPTARG"
      ;;
  esac
done

if [[ "$UTILITY" = "make" ]]; then
  make clean
 
  if $BEAR; then
    bear -- make $BUILD_TYPE
  else
    make $BUILD_TYPE
  fi
elif [[ "$UTILITY" = "cmake" ]]; then
  # Remove the build directory if it exists
  rm -rf build

  # If the -b flag is provided, we enable bear
  if $BEAR; then
    cmake -S . -B build -DCMAKE_BUILD_TYPE="$BUILD_TYPE" -DCMAKE_TOOLCHAIN_FILE=~/vcpkg/scripts/buildsystems/vcpkg.cmake
    bear -- cmake --build build
  else
    cmake -S . -B build -DCMAKE_BUILD_TYPE="$BUILD_TYPE" -DCMAKE_TOOLCHAIN_FILE=~/vcpkg/scripts/buildsystems/vcpkg.cmake
    cmake --build build
  fi
fi
