# Real-Time System Monitor

An embedded Qt 6/QML system monitor for a 320x240 ILI9341 SPI framebuffer and
XPT2046 touch controller on Raspberry Pi 3 64-bit.

The repository is intentionally a manual bring-up project. Yocto builds a
lightweight `core-image-base`; the overlay, custom kernel modules, framebuffer
diagnostic, and cross-built app are deployed separately through SCP.

## Repository Map

- `app/`: Qt 6 C++/QML application exposed through the `systemInfo` facade.
- `yocto/meta-rpi-ili9341-bsp/`: device tree, display/touch modules, and
  framebuffer diagnostic recipes.
- `yocto/config/rpi3-ili9341-bringup/`: copyable `local.conf` and optional SDK
  configuration; layers are registered with `bitbake-layers`.
- `docs/`: source-derived architecture, BSP, deployment, and verification
  guidance.

## Validation Status

Source-level checks can validate the Qt app, device tree syntax, and driver
style. Color output, touch behavior, Wi-Fi persistence, and end-to-end app
interaction remain board-only evidence.

Start with [docs/00-start-here.md](docs/00-start-here.md).
