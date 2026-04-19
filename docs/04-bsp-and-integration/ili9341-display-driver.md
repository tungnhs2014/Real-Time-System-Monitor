# ILI9341 Display Driver

## Primary Source
`yocto/meta-rpi-ili9341-bsp/recipes-kernel/ili9341-fb/files/ili9341_fb.c`

## Driver Role
Framebuffer-oriented display driver for the ILI9341 panel over SPI.

## Responsibilities
- initialize the display panel
- convert framebuffer pixel data
- transmit display updates over SPI
- use control GPIOs for command/data, reset, and backlight

## Naming Rationale
The chosen name emphasizes what the driver actually is:
- device-specific: ILI9341
- function-specific: framebuffer display driver
- transport is SPI, but the driver is not a generic SPI controller driver
