# meta-rpi-ili9341-bsp

This layer contains the Raspberry Pi BSP-side integration for the System Monitor product.

## Scope
- Raspberry Pi device tree overlay for SPI display and touch
- ILI9341 framebuffer display driver module
- XPT2046 touch driver module
- Linux kernel patch/config fragments needed by the display path
- bring-up helper recipes kept for local validation

## Main Paths
- `conf/`
- `recipes-bsp/rpi-ili9341-xpt2046-dtbo/`
- `recipes-kernel/ili9341-fb/`
- `recipes-kernel/xpt2046-touch/`
- `recipes-kernel/linux/`

## Notes
- This is a Yocto layer, not just a driver folder.
- `recipes-apps/` contains bring-up helpers and is not the main product application packaging path.
