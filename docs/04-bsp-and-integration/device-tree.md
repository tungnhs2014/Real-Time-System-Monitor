# Device Tree

## Primary Source
`yocto/meta-rpi-ili9341-bsp/recipes-bsp/rpi-ili9341-xpt2046-dtbo/files/rpi-ili9341-xpt2046-overlay.dts`

## Responsibilities
- enable SPI bus usage for the target devices
- bind the display and touch nodes
- define GPIO usage
- provide runtime override parameters such as speed and rotation

## Review Focus
- chip-select assignments
- GPIO polarity and interrupt configuration
- calibration-related properties
- alignment with display and touch driver expectations
