# Yocto Layer

## Active BSP Layer
`yocto/meta-rpi-ili9341-bsp/`

Contains:
- overlay recipe
- display driver recipe
- touch driver recipe
- kernel patch/config support
- bring-up helper recipes

## Product Layer
`yocto/meta-system-monitor/`

Contains:
- product-side packaging scaffold for the main application

## Why This Split Is Preferred
- aligns with common Yocto layer conventions
- avoids calling a full layer merely `drivers`
- keeps board support and product packaging conceptually separate
