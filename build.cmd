@echo off
setlocal enabledelayedexpansion

rem equafun Project build script for Windows
rem Similar to build.sh, but I removed the utility and bear option because:
rem utility doesn't matter here as right now for compilation on Windows only CMake is used
rem bear doesn't matter as there's no support for bear on Windows right now

rem Default value
set type="Release"

rem Parse the arguments
:parseArgs
rem if there's nothing left to parse
if "%~1"=="" goto done

rem parse current argument
rem for example, if we have this as an argument list: -t "Debug"
rem then arg will be set to "-t"
set arg=%~1

rem the argument list will be shifted, so now it'll look like this: "Debug"
shift

rem Check for each flag
if "%arg%"=="-t" (
  rem and here we set the type to the option value

  rem check if the next argument is valid
  powershell -Command "if ('%arg%' -match '^(Debug|Release|MinSizeRel|Flags)$') { exit 0 } else { exit 1 }"

  rem check whether the powershell command succeeded or not
  if %errorlevel% equ 0 (
    rem ~ removes the double quotes from the option value
    set type=%~1
    shift
  ) else (
    echo Invalid build type provided: %arg%
    echo Check '.\build.cmd -h' for valid build types
    exit /b 1
  )
) else if "%arg%"=="-h" (
  rem display help
  echo Equafun project build script (Windows)
  echo Script version: 0.0.1
  echo Latest update: Alpha v0.0.6
  echo Options:
  echo   -h             | display this help
  echo   -t [buildType] | provide a build type to the compile utility
  echo     Valid options are: Debug, Release, MinSizeRel, Flags"
  echo Example use case: '.\build.cmd -t "Debug"'
  echo Note: without any arguments, the script defaults to using %type% as the build type.
) else (
  echo Unknown option: %arg%
  exit /b 1
)

goto parseArgs

rem we're done parsing the arguments
:done

rd /s /q build
cmake -S . -B build -G "Ninja" -DCMAKE_BUILD_TYPE=%type%
cmake --build build
exit /b 0
