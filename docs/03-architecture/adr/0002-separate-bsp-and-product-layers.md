# ADR 0002: Separate BSP Layer From Product Layer

## Status
Accepted

## Context
The active Yocto content contains both BSP-side assets and future product-side packaging concerns. Treating the whole area as only `drivers` is misleading.

## Options Considered
- keep one mixed layer for everything
- separate BSP concerns from product-layer concerns

## Decision
Use:
- `yocto/meta-rpi-ili9341-bsp/` for active hardware-facing assets
- `yocto/meta-system-monitor/` for product packaging

## Tradeoffs
- product-side layer starts as scaffold before full packaging is implemented
- naming and documentation become much clearer

## Consequences
- reviewers can inspect BSP and product packaging independently
- future app recipe work has a clear home
