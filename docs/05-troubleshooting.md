# Troubleshooting

## App Does Not Build Locally

Check that Qt6 development packages are installed and discoverable by CMake. The app needs Qt6 Core, Gui, Quick, and Qml.

## GPU Detail Page Is Missing

This is current behavior. The dashboard shows GPU summary metrics, but the app does not implement a dedicated GPU detail page.

## Settings Look Partially Implemented

Threshold editing is the clearest completed settings flow. Some backend settings exist for persistence but are not fully surfaced as complete UI workflows.

## Product Layer Is Not Release-Proven Yet

`meta-system-monitor/` contains the app recipe, service, and product image recipe. It still needs `bitbake` validation and board boot validation before release use.

## Touch Coordinates Are Wrong

On the target, collect evidence first:

```sh
dmesg | grep -Ei 'xpt2046|ads7846|touch|spi'
evtest
QT_LOGGING_RULES='qt.qpa.*=true' system-monitor
```

Then adjust only the overlay calibration/orientation flags needed for the mounted display orientation.

## Touch Interrupt Storm

Confirm the deployed overlay matches the active-low PENIRQ wiring and driver expectation. Do not change interrupt polarity without board wiring evidence and `dmesg` logs.

## Display Does Not Appear

Check:

```sh
dmesg | grep -Ei 'ili9341|fb|spi'
ls -l /dev/fb*
```

Then confirm SPI, GPIO, overlay loading, module autoload names, and Qt framebuffer environment are aligned with the target image.
