# meta-rpi-ili9341-bsp

Raspberry Pi BSP layer for an ILI9341 SPI framebuffer display and XPT2046
touch controller.

## Scope

- Device tree overlay for SPI0 CE0 display and SPI0 CE1 touch.
- ILI9341 framebuffer display module: `ili9341_display.ko`.
- XPT2046 touch input module: `ili9341_touch.ko`.
- Kernel patch/config fragments needed by the fbdev deferred I/O path.
- Board-side test utilities used during manual bring-up.

## Main Paths

- `conf/`
- `recipes-bsp/ili9341-xpt2046-overlay/`
- `recipes-kernel/ili9341-display/`
- `recipes-kernel/ili9341-touch/`
- `recipes-kernel/linux/`
- `recipes-test/framebuffer-test/`

## Bring-Up Workflow

Build driver artifacts first and copy them to the board with SCP:

```sh
bitbake ili9341-display ili9341-touch ili9341-xpt2046-overlay framebuffer-test
```

The expected artifacts are:

- `ili9341_display.ko`
- `ili9341_touch.ko`
- `ili9341-xpt2046-overlay.dtbo`
- `fb_test`

For a display module copied to `/tmp`, install it in the kernel module tree
and use `modprobe`; this loads its fbdev dependencies automatically:

```sh
modprobe -r ili9341_display 2>/dev/null || true
install -D -m 0644 /tmp/ili9341_display.ko \
  /lib/modules/$(uname -r)/extra/ili9341_display.ko
depmod -a
modprobe ili9341_display
```

The base image intentionally excludes the overlay, modules, framebuffer test,
and application. Deploy each artifact manually through SCP during validation.

## Notes

- This is a Yocto layer, not a loose driver folder.
- Copy `yocto/config/rpi3-ili9341-bringup/local.conf` into local build state
  and register this layer with `bitbake-layers add-layer`.
