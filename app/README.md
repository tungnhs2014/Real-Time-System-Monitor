# App

This directory contains the userspace System Monitor application.

## Main Areas
- `src/`: C++ backend and controller logic
- `qml/`: UI shell, pages, and reusable components
- `resources/`: Qt resource manifest
- `assets/`: icons and UI assets
- `tests/`: reserved space for automated tests

## Entry Points
- build entry: `app/CMakeLists.txt`
- runtime bootstrap: `app/src/main.cpp`
- UI shell: `app/qml/Main.qml`
