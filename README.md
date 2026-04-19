# System Monitor

System Monitor is an embedded Qt/QML monitoring application for Raspberry Pi-class hardware using an ILI9341 SPI display and XPT2046 touch input.

The repository contains:
- a Qt/QML userspace monitoring application
- a Yocto BSP layer for display, touch, and kernel-side integration
- a product-side Yocto layer for app and image packaging
- compact source-of-truth documentation under `docs/`

## Why This Repository Exists
- provide a dedicated on-device system monitor UI
- keep app code, BSP integration, and design documentation in one versioned place
- support both desktop-oriented validation and Raspberry Pi bring-up

## Repository Map
- `app/`: userspace application
- `yocto/meta-rpi-ili9341-bsp/`: active BSP integration layer
- `yocto/meta-system-monitor/`: product-side application and image packaging
- `docs/`: product, architecture, BSP/Yocto, build/test, troubleshooting, diagrams

## Start Here
- reading map: [docs/00-start-here.md](docs/00-start-here.md)
- product scope: [docs/01-product.md](docs/01-product.md)
- runtime architecture: [docs/02-architecture.md](docs/02-architecture.md)
- build, run, and test: [docs/04-build-run-test.md](docs/04-build-run-test.md)

## Quick Start
- understand the repository first through [docs/00-start-here.md](docs/00-start-here.md)
- inspect the app under `app/`
- inspect BSP integration under `yocto/meta-rpi-ili9341-bsp/`
- inspect product packaging under `yocto/meta-system-monitor/`

## Build Entry Points
- repository root wrapper: `CMakeLists.txt`
- application build file: `app/CMakeLists.txt`
- Yocto BSP layer: `yocto/meta-rpi-ili9341-bsp/`
- Yocto product layer: `yocto/meta-system-monitor/`

## Documentation Rule
README is intentionally concise.

Detailed product scope, architecture, diagrams, build/test notes, and troubleshooting live in `docs/`.
