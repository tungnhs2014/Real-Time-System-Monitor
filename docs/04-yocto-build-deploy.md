# Yocto Build and Deploy

This project builds a light `core-image-base`, then deploys the ILI9341/XPT2046
artifacts and application manually. `poky/build/conf` is local workspace state;
the tracked configuration is `yocto/config/rpi3-ili9341-bringup/`.

## Configure a Build Directory

The documented host layout is:

```text
TungNHS/
  Yocto-pi3plus/poky/
  Qt-Project/ili9341-system/
```

Initialize the usual Poky build directory, copy the project configuration, and
let `bitbake-layers` record workspace-specific layer paths:

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
bitbake -p
```

Expected result: `meta-rpi-ili9341-bsp` is the only project layer, registered
from `PROJECT_DIR`. `bblayers.conf` is intentionally not versioned because its
absolute paths belong to the local host.

## Build Artifacts

Build the base image and board artifacts independently so failures stay local
to the responsible recipe.

```sh
bitbake core-image-base
bitbake ili9341-xpt2046-overlay
bitbake ili9341-display
bitbake ili9341-touch
bitbake framebuffer-test
```

The image includes Qt runtime, Wi-Fi, SSH/SCP, `kmod`, fbdev helper modules,
`evtest`, and `fbset`. It deliberately excludes the custom overlay, custom
drivers, framebuffer test, and application.

## Optional Qt6 SDK

The SDK adds host/target development tasks only; it does not increase rootfs
size. Copy the optional include and add one `require sdk.inc` line to the local
configuration before generating it:

```sh
cp "$PROJECT_DIR/yocto/config/rpi3-ili9341-bringup/sdk.inc" conf/sdk.inc
printf '\nrequire sdk.inc\n' >> conf/local.conf
bitbake core-image-base -c populate_sdk
```

Run the generated installer in `tmp/deploy/sdk/`, source its environment, then
cross-build the app:

```sh
source /opt/poky/<version>/environment-setup-cortexa53-poky-linux

cmake -S "$PROJECT_DIR/app" -B build-rpi \
  -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_PREFIX_PATH="$OECORE_TARGET_SYSROOT/usr" \
  -DCMAKE_FIND_ROOT_PATH="$OECORE_TARGET_SYSROOT" \
  -DCMAKE_FIND_ROOT_PATH_MODE_PACKAGE=ONLY
cmake --build build-rpi -j"$(nproc)"
file build-rpi/system-monitor
```

The final command must report an AArch64 executable.

## Flash and Access the Board

Confirm the SD-card device with `lsblk`, then flash the generated image:

```sh
sudo dd if=tmp/deploy/images/raspberrypi3-64/core-image-base-raspberrypi3-64.rpi-sdimg \
  of=/dev/sdX bs=4M status=progress conv=fsync
sync
```

For isolated bring-up, `debug-tweaks` permits UART or SSH login as `root` with
an empty password. UART uses `115200 8N1`. This is not a hardened deployment
policy.

## Configure Wi-Fi Once

Keep credentials only on the board. The configuration and enabled services are
stored in writable `/etc` and therefore survive reboot.

```sh
mkdir -p /etc/wpa_supplicant
cat > /etc/wpa_supplicant/wpa_supplicant-wlan0.conf <<'EOF'
ctrl_interface=/run/wpa_supplicant
update_config=0
country=VN
EOF

wpa_passphrase "<SSID>" "<PASSWORD>" >> /etc/wpa_supplicant/wpa_supplicant-wlan0.conf
chmod 600 /etc/wpa_supplicant/wpa_supplicant-wlan0.conf
systemctl enable --now wpa_supplicant@wlan0.service
systemctl enable --now dhcpcd@wlan0.service
ip addr show wlan0
```

## Deploy Overlay and Modules

Copy the compiled `.dtbo` to the Raspberry Pi FAT boot partition. Add this to
its `config.txt`, then reboot so the SPI devices are created:

```text
dtoverlay=ili9341-xpt2046-overlay
```

Copy modules to the running kernel module tree. `depmod` lets `modprobe` load
the installed fbdev dependencies automatically.

```sh
scp <host-path>/ili9341_display.ko root@<board-ip>:/tmp/
scp <host-path>/ili9341_touch.ko root@<board-ip>:/tmp/

install -D -m 0644 /tmp/ili9341_display.ko \
  /lib/modules/$(uname -r)/extra/ili9341_display.ko
install -D -m 0644 /tmp/ili9341_touch.ko \
  /lib/modules/$(uname -r)/extra/ili9341_touch.ko
depmod -a
modprobe ili9341_display
modprobe ili9341_touch

dmesg | grep -Ei 'ili9341|xpt2046|spi|fb|touch|error|fail'
ls -l /dev/fb* /dev/input/event* 2>/dev/null
```

## Deploy Diagnostics and App

```sh
scp <host-path>/fb_test root@<board-ip>:/tmp/
scp build-rpi/system-monitor root@<board-ip>:/tmp/

chmod 0755 /tmp/fb_test /tmp/system-monitor
/tmp/fb_test /dev/fb1
QT_QPA_PLATFORM=linuxfb QT_QPA_FB=/dev/fb1 /tmp/system-monitor
```

The app startup code already selects `/dev/fb1` and one `evdevtouch` backend;
the command keeps the intended framebuffer explicit during bring-up.
