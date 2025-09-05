# Win11Demo

This repository contains a minimal Win32 and OpenGL 2.1 demo.

## Overview

- `intro.cpp` provides a small Win32 + OpenGL program that loads and hot-reloads a fragment shader at runtime.
- `shader.frag` is a sample shader that renders a simple animated tunnel effect.
- `build.bat` is a helper script for compiling `intro.cpp` with the Microsoft Visual C++ toolchain.

## Building

### Visual Studio

Open `Win11Demo.sln` in Visual Studio and build the solution.

### Command line

From a Visual Studio Developer Command Prompt, run:

```bat
build.bat
```

This produces `intro.exe`.

## Running

Run the generated executable. Press `R` to reload the shader, `Alt+Enter` to toggle fullscreen, and `Esc` to quit.

## Requirements

- Windows with OpenGL drivers
- Microsoft Visual C++ build tools (for `cl` and related libraries)

