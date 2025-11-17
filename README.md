# Equafun (Alpha v0.0.6)

## Features
- Graph render with labeled markers
- Full fledged lexer, parser and evaluator for function definition handling
- Function rendering
- Adjusting the window size dynamically grows/shrinks the graph and values still match

## Changelog
- The full changelog for the project can be found [here](docs/changelog.md).

## Note
- Support for **Linux** and **Windows** as of `Alpha v0.0.6`.

## How to build from the beginning & run
- **IMPORTANT: ** you *have* to pass the functions you want to be rendered as arguments, as shown in the example here: 
    - `./build/equafun "f(x) = x" "g(x) = x^2"` (running the executable on Linux)
    - (function identifiers don't have to be the same as in the example, but they have to be unique)
- **The resulting binary is in `build`** 
- To run the project, either:
    1. Go to *build* and run the executable there (*equafun(.exe)*)
    2. Run the executable from the root directory using *./build/equafun(.exe)*

### Linux:
#### Prerequisites
- **Required:** *make/Cmake*, *gcc*
    > if using make, *glfw* and *freetype* is required
    > if using Cmake, *vcpkg* is required
- **OpenGL version 3.3 or above**

#### Build Using Make
1. Simply run `./build.sh make` in the root directory of the project. The script runs `make clean` followed by `make` which compiles the project.

#### Build Using Make + Bear
1. **Additional prerequisite:** *bear*
2. Simply run `./build.sh make -b` in the root directory of the project. The script runs `make clean`, followed by `bear -- make` to compile the project while also generating `compile_commands.json`.

#### Build Using CMake
1. **Additional prerequisite:** *vcpkg*
    > installed in ~/vcpkg (ill change it soon)
2. Simply run `./build.sh cmake` in the root directory of the project. The script removes the build directory and compiles the project from the beginning.

#### Build using CMake + Bear
1. **Additional prerequisite:** *vcpkg, bear*
    > installed in ~/vcpkg (ill change it soon)
2. Simply run `./build.sh cmake -b` in the root directory of the project. The script removes the build directory and compiles the project from the beginning.

#### **Repeated build**
- after building it for the first time with one of the above commands, you can use either
    1. *make*
    2. *cmake --build build*
- to rebuild only the files that were changed, including changes only to the header files

### Windows

#### Prerequisites
- **Required:** *Cmake*, *mingw*, *ninja*, *vcpkg*
- **OpenGL version 3.3 or above**

#### Build using CMake
1. **Additional preresquisites:**
    > properly set enviroment variables for vcpkg
    > ninja installed and set up for C23
2. Simply run `.\build.cmd` in the root directory of the project. The script removes the build directory and compiles the project from the beginning

#### **Repeated build**
- after building it for the first time with one of the above commands, you can use 
    1. *cmake --build build*
- to rebuild only the files that were changed, including changes only to the header files
