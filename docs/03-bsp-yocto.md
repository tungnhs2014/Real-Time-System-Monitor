# BSP And Yocto

## Hardware Target

The target hardware family is Raspberry Pi-class, including Raspberry Pi Zero-style boards, with:

- ILI9341 display on SPI0.0
- XPT2046 touch controller on SPI0.1
- GPIO lines for display DC/reset/backlight and touch PENIRQ

## BSP Layer

`yocto/meta-rpi-ili9341-bsp/` owns hardware-specific integration:

- `recipes-bsp/rpi-ili9341-xpt2046-dtbo/`: device tree overlay.
- `recipes-kernel/ili9341-fb/`: ILI9341 framebuffer-style SPI display module.
- `recipes-kernel/xpt2046-touch/`: XPT2046 Linux input touch module.
- `recipes-kernel/linux/`: Raspberry Pi kernel patch/config support.

The BSP layer should not own product app packaging.

## Product Layer

`yocto/meta-system-monitor/` owns product-specific packaging:

- `recipes-apps/system-monitor/system-monitor_4.0.0.bb`: Qt/QML app recipe.
- `recipes-apps/system-monitor/files/system-monitor.service`: systemd unit.
- `recipes-core/images/system-monitor-image.bb`: product image composition.

The current app recipe uses `externalsrc` so it builds from the repository workspace. That is useful during development, but release validation still requires a real target Yocto build.

## Device Tree Contract

The overlay should:

- enable SPI0
- disable default `spidev` children when binding the real display/touch devices
- bind `ilitek,ili9341` on chip select 0
- bind `xptek,xpt2046` on chip select 1
- describe display GPIOs, touch PENIRQ polarity, calibration, orientation flags, and SPI speeds

## Driver Documentation Rule

Linux driver C should use kernel-doc for important structs, callbacks, hardware assumptions, and device-tree behavior. Do not use Doxygen-style Qt/C++ comments in kernel C.

## Validation

Local checks:

```sh
dtc -@ -I dts -O dtb -o /tmp/rpi-ili9341-xpt2046-overlay.dtbo \
  yocto/meta-rpi-ili9341-bsp/recipes-bsp/rpi-ili9341-xpt2046-dtbo/files/rpi-ili9341-xpt2046-overlay.dts
```

Target Yocto checks:

```sh
bitbake rpi-ili9341-xpt2046-dtbo
bitbake ili9341-fb
bitbake xpt2046-touch
bitbake system-monitor
bitbake system-monitor-image
```

Board checks:

```sh
dmesg | grep -Ei 'ili9341|xpt2046|spi|fb|touch'
ls -l /dev/fb*
ls -l /dev/input/event*
```
