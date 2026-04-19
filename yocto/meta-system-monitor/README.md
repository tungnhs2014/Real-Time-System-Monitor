# meta-system-monitor

This layer is reserved for product-side packaging of the System Monitor application.

## Intended Scope
- application recipe for the Qt/QML monitor
- product image/package configuration
- product-level integration that should stay separate from BSP concerns

## Current State
- scaffold only
- application is currently built from `app/` with CMake
- no final Yocto recipe for the main monitor app is committed yet
