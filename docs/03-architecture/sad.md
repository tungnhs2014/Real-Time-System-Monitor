# System Architecture Document (SAD)

## Scope
This SAD describes the repository-level architecture for the brownfield System Monitor product.

It is architecture-facing, not implementation-complete. For current runtime truth, see `docs/03-architecture/as-is-architecture.md`.

## System Context
The product combines:
- a Qt/QML userspace monitor application
- Raspberry Pi BSP integration for display and touch
- Yocto layer structure for deployment and packaging

## Architecture Boundaries

### Zone 1: Product application
- `app/`
- Qt/QML frontend plus C++ backend

### Zone 2: BSP integration
- `yocto/meta-rpi-ili9341-bsp/`
- device tree, kernel drivers, kernel patch/config support

### Zone 3: Product packaging layer
- `yocto/meta-system-monitor/`
- application packaging and product-level Yocto integration

## Current-State Baseline
- `SystemController` remains the primary aggregation facade for QML
- monitor-specific logic remains in model classes
- display and touch remain delivered through the BSP layer
- main page flow remains primarily in `app/qml/Main.qml`
- `NavigationController` is present but is not the dominant runtime navigation path

## Transition Architecture Direction
- keep one monorepo
- keep the current app functional while documentation and structure are normalized
- document `as-is`, `transition`, and `target` separately
- evolve toward clearer separation of product-layer packaging and BSP concerns without forcing a full runtime rewrite first

## Target Architecture Direction
- keep app, BSP, and product packaging zones explicit
- complete a reviewable product-side packaging path
- allow runtime modularization to happen incrementally only after the current brownfield truth is fully documented

## Major Relationships
- the app consumes Linux runtime information and renders the UI
- the BSP layer provides display and touch capability to the target runtime
- the product layer is responsible for packaging the application as a product artifact

## Supporting Repository Artifacts
The repository also contains supporting assets that are important for engineering governance but are not runtime architecture zones:
- `docs/`
- `private-docs/ai/`

## Supporting Views
- `docs/03-architecture/as-is-architecture.md`
- `docs/03-architecture/diagrams/runtime-component-diagram.md`
- `docs/03-architecture/diagrams/packaging-and-deployment-diagram.md`
- `docs/03-architecture/diagrams/use-case-diagram.md`
- `docs/03-architecture/diagrams/sequence-diagram.md`
- `docs/03-architecture/deployment-view.md`
