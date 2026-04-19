# System Monitor

System Monitor is a brownfield full-stack embedded product repository for Raspberry Pi-class hardware using an ILI9341 SPI display and XPT2046 touch input.

The repository contains:
- a Qt/QML userspace monitoring application
- a Yocto BSP layer for display, touch, and kernel-side integration
- a product-side Yocto layer scaffold
- public design documentation under `docs/`

## Why This Repository Exists
- provide a dedicated on-device system monitor UI
- keep app code, BSP integration, and design documentation in one versioned place
- support both desktop-oriented validation and Raspberry Pi bring-up

## Repository Map
- `app/`: userspace application
- `yocto/meta-rpi-ili9341-bsp/`: active BSP integration layer
- `yocto/meta-system-monitor/`: product-side packaging scaffold
- `docs/`: requirements, solution analysis, architecture, diagrams, verification

## Start Here
- project overview: [docs/01-product/overview.md](docs/01-product/overview.md)
- design reading map: [docs/00-start-here.md](docs/00-start-here.md)
- selected solution: [docs/02-solution/selected-solution.md](docs/02-solution/selected-solution.md)
- system architecture: [docs/03-architecture/sad.md](docs/03-architecture/sad.md)

## Quick Start
- understand the repository first through [docs/00-start-here.md](docs/00-start-here.md)
- review product context in [docs/01-product/overview.md](docs/01-product/overview.md)
- inspect the app under `app/`
- inspect BSP integration under `yocto/meta-rpi-ili9341-bsp/`

## Build Entry Points
- repository root wrapper: `CMakeLists.txt`
- application build file: `app/CMakeLists.txt`
- Yocto BSP layer: `yocto/meta-rpi-ili9341-bsp/`

## Documentation Rule
README is intentionally concise.

Detailed requirements, architecture, diagrams, decisions, and verification notes live in `docs/`.
