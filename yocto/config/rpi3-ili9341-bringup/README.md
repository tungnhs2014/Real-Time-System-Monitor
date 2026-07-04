# Raspberry Pi 3 ILI9341 Bring-Up Configuration

This directory contains configuration copied into a local Poky build directory.
It does not contain `bblayers.conf`: register layers with `bitbake-layers` so
BitBake records paths for the current workspace.

```sh
cd /home/tungnhs/TungNHS/Yocto-pi3plus/poky
source oe-init-build-env build

PROJECT_DIR=/home/tungnhs/TungNHS/Qt-Project/ili9341-system
cp "$PROJECT_DIR/yocto/config/rpi3-ili9341-bringup/local.conf" conf/local.conf

bitbake-layers remove-layer \
  /home/tungnhs/TungNHS/Yocto-pi3plus/meta-rpi-ili9341-bsp 2>/dev/null || true

bitbake-layers add-layer \
  "$OEROOT/meta-openembedded/meta-oe" \
  "$OEROOT/meta-openembedded/meta-python" \
  "$OEROOT/meta-openembedded/meta-networking" \
  "$OEROOT/meta-raspberrypi" \
  "$OEROOT/meta-qt6" \
  "$PROJECT_DIR/yocto/meta-rpi-ili9341-bsp"

bitbake-layers show-layers
```

For an SDK, copy `sdk.inc` to `conf/sdk.inc`, add exactly one
`require sdk.inc` line to `conf/local.conf`, then run:

```sh
bitbake core-image-base -c populate_sdk
```

The image intentionally excludes the custom overlay, display/touch modules,
framebuffer test, and application. Build and deploy those artifacts manually.
