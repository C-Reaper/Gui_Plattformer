# Project README

## Overview
This project is a C program designed to visualize and simulate collision detection between different geometric shapes. The main functionality allows users to create, move, and interact with various shapes on a graphical interface.

## Features
- Creation of basic geometric shapes (e.g., circles, rectangles).
- Movement and manipulation of these shapes.
- Real-time visualization of shape interactions and collisions.

## Project Structure
```
Gui_Shapes_Intersection/
├── build/              # .exe files produced by Main.c
├── src/
│   ├── Main.c          # Entry point
├── Makefile.linux      # Linux Build configuration
├── Makefile.windows    # Windows Build configuration
├── Makefile.wine       # Wine Build configuration
└── Makefile.web        # Emscripten Build configuration
└── README.md           # This file
└── LICENCE
└── .gitignore
```

### Prerequisites
- C/C++ Compiler and Debugger (GCC, Clang)
- Make utility
- Standard development tools
- Libraries needed in specific projects (X11 for GUI)

## Build & Run
To build and run the project:

### Linux
```bash
cd Gui_Shapes_Intersection/
make -f Makefile.linux all
./build/Main
```

### Windows
```bash
cd Gui_Shapes_Intersection/
make -f Makefile.windows all
build\Main.exe
```

### Wine (Cross Compile for Windows)
```bash
cd Gui_Shapes_Intersection/
make -f Makefile.wine all
wine build/Main.exe
```

### WebAssembly (Emscripten)
```bash
cd Gui_Shapes_Intersection/
make -f Makefile.web all
emrun --no_browser --port 8080 build/index.html
```

For clean rebuild:
```bash
make -f Makefile.linux clean
make -f Makefile.linux all
```

For debug:
```bash
make -f Makefile.linux do
```

This README provides a clear guide on how to build and run the project across different platforms, ensuring that all necessary components are included for each environment.