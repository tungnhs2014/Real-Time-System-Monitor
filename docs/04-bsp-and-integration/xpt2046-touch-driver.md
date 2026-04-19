# XPT2046 Touch Driver

## Primary Source
`yocto/meta-rpi-ili9341-bsp/recipes-kernel/xpt2046-touch/files/xpt2046_touch.c`

## Driver Role
SPI touch controller driver that reports Linux input events.

## Responsibilities
- handle pen-down event flow
- sample touch coordinates and pressure
- apply calibration and axis transforms
- report input events to userspace
