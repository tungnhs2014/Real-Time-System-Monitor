# Build And Run

## Purpose

This document records the current build and run entry points without turning README into a full operations manual.

## Current Desktop-Oriented Path

Repository entry point:
- root `CMakeLists.txt`

Application entry point:
- `app/CMakeLists.txt`

Current desktop flow:
1. configure the project from the repository root
2. build the `app/` target through CMake
3. run the Qt/QML application in a desktop environment for smoke validation

## Current Target-Oriented Path

Current target flow:
1. integrate `yocto/meta-rpi-ili9341-bsp/` into the Yocto environment
2. build the overlay and kernel modules for display and touch
3. bring up framebuffer and touch input on target hardware
4. package the main application through the product layer when that path is completed

## Current Build Reality

- desktop validation is the most direct way to iterate on the app
- target validation is still required for real display and touch behavior
- the final Yocto product packaging path for the app is not yet completed in `meta-system-monitor/`
