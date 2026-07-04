# Yocto BSP

This directory contains only the versioned Raspberry Pi BSP layer. It does not
contain Poky, build output, downloads, shared-state cache, an app package, or
a release image recipe.

`meta-rpi-ili9341-bsp/` provides the ILI9341/XPT2046 overlay, display and touch
modules, kernel support, and the framebuffer diagnostic recipe. The copyable
bring-up configuration is under `config/rpi3-ili9341-bringup/`.

Use the manual workflow in [../docs/04-yocto-build-deploy.md](../docs/04-yocto-build-deploy.md).
