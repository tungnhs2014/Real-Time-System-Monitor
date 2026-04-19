# Deployment View

## Desktop Validation Path
- configure and build from repository root using the wrapper `CMakeLists.txt`
- run the Qt app in a desktop environment for iteration and smoke validation

## Target Deployment Path
- apply BSP layer content through Yocto
- deploy overlay and kernel modules for display and touch
- package the app through the product layer when that path is completed

## Deployment Units
- Qt application binary and assets
- device tree overlay
- ILI9341 display module
- XPT2046 touch module
- Linux patch/config support where required

## Environments
- desktop development environment
- Raspberry Pi target environment
