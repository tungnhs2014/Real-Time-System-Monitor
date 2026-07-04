# Hardware and BSP

The validated target is Raspberry Pi 3 64-bit using the
`ili9341-xpt2046-overlay` on SPI0.

| Function | Device-tree binding | Value |
| --- | --- | --- |
| ILI9341 display | SPI0 CE0 | `reg = <0>`, 32 MHz |
| XPT2046 touch | SPI0 CE1 | `reg = <1>`, 2 MHz |
| Display data/command | `dc-gpios` | GPIO24 |
| Display reset | `reset-gpios` | GPIO25 |
| Display backlight | `led-gpios` | GPIO18 |
| Touch PENIRQ | `pendown-gpios` | GPIO17, active low, falling IRQ |

The overlay removes the default SPI `spidev` nodes and creates nodes with
`ilitek,ili9341` and `xptek,xpt2046` compatible strings. `modprobe` binds the
manual modules only after the Raspberry Pi firmware has loaded the overlay at
boot; it cannot create missing device-tree nodes.

## Display Contract

`ili9341_display.ko` publishes a 320x240 RGB565 framebuffer. fbdev stores
RGB565 little-endian; the driver only swaps byte order while sending SPI data.
It does not swap red and blue in software. The ILI9341 panel uses the MADCTL
BGR-order bit so an RGB565 red pixel remains red on this panel.

`rotation` accepts `0`, `90`, `180`, or `270`. `fps` is read from the overlay;
only `1..60` is accepted and invalid/missing values use `30`. It controls the
fbdev deferred-I/O refresh delay.

## Touch Contract

The active-low PENIRQ falling edge schedules delayed work outside interrupt
context. Each poll reads five X/Y/Z samples, uses the median values, rejects
insufficient pressure, applies DT calibration/orientation, suppresses movement
below eight coordinate units, and reports release after two consecutive pen-up
polls. The poll interval is 20 ms.

Calibration properties are 32-bit DT cells because the driver reads them with
`of_property_read_u32()`. The current values are X/Y `200..3900`, pressure
`30..255`, with `ti,swap-xy`, `ti,invert-x`, and `ti,invert-y` enabled.

## Manual Lifecycle

1. Copy the compiled overlay to the FAT boot partition and enable
   `dtoverlay=ili9341-xpt2046-overlay` in `config.txt`.
2. Reboot so SPI nodes exist.
3. Copy modules into `/lib/modules/$(uname -r)/extra/`, run `depmod -a`, then
   run `modprobe ili9341_display` and `modprobe ili9341_touch`.
4. Verify `/dev/fb1` and `/dev/input/event*`, then launch the app.

The bind, display flush, and touch event paths are shown in
[diagrams.md](diagrams.md).
