# Equafun (Alpha v0.0.2)

## Features
- Opens a window and prints some messages into the console

## Changelog
- The full changelog for the project can be found [here](docs/changelog.md).

## Note
- Only **LINUX** support as of `Alpha v0.0.2`.

## How to build & run

- **The resulting binary is in `build`. To start the project, run `./build/equafun`.**

### Prerequisites
- **Required:** *make*, *gcc*, *glfw*
- **OpenGL version 3.3 or above**

### Build Using Make
1. Simply run `make` in the root directory of the project.

### Build Using Make + Bear
1. **Additional prerequisite:** *bear*
2. Simply run `./build.sh` in the root directory of the project. The script runs `make clean`, followed by `bear -- make` to compile the project while also generating `compile_commands.json`.
