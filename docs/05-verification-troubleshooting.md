# Verification and Troubleshooting

Keep source, Yocto, and board evidence separate. A successful host build does
not prove display colors, touch quality, or Wi-Fi persistence on hardware.

## Evidence Matrix

| Area | Evidence | Status type |
| --- | --- | --- |
| Qt/QML | `qmllint` and CMake build | Local-only |
| Device tree | `dtc` overlay compilation | Local-only |
| Driver style | `checkpatch.pl` when provided by kernel sources | Local-only |
| Yocto recipes | `bitbake -p` and individual recipe builds | Yocto build evidence |
| Display colors | `fb_test /dev/fb1` | Board-only |
| Touch filtering | `evtest /dev/input/eventX` | Board-only |
| App interaction | `system-monitor` on `/dev/fb1` | Board-only |
| Wi-Fi persistence | two reboot cycles | Board-only |

## Local Checks

```sh
git diff --check

qmllint -I app/qml <changed-qml-files>
cmake -S app -B /tmp/system-monitor-check -DCMAKE_BUILD_TYPE=Debug
cmake --build /tmp/system-monitor-check -j"$(nproc)"

dtc -@ -I dts -O dtb -o /tmp/ili9341-xpt2046-overlay.dtbo \
  yocto/meta-rpi-ili9341-bsp/recipes-bsp/ili9341-xpt2046-overlay/files/ili9341-xpt2046-overlay.dts

checkpatch.pl --file yocto/meta-rpi-ili9341-bsp/recipes-kernel/ili9341-display/files/ili9341_display.c
checkpatch.pl --file yocto/meta-rpi-ili9341-bsp/recipes-kernel/ili9341-touch/files/ili9341_touch.c
```

`checkpatch.pl` is normally supplied by a kernel source tree. These checks do
not establish formal MISRA or CERT compliance.

## Yocto Checks

After copying `local.conf` and registering layers with `bitbake-layers`:

```sh
bitbake -p
bitbake-layers show-layers
bitbake core-image-base
bitbake ili9341-xpt2046-overlay
bitbake ili9341-display
bitbake ili9341-touch
bitbake framebuffer-test
```

`meta-rpi-ili9341-bsp` must be the only project layer. The source tree has no
application package or product image recipe.

## Board Acceptance

```sh
dmesg | grep -Ei 'ili9341|xpt2046|spi|fb|touch|error|fail'
ls -l /dev/fb* /dev/input/event* 2>/dev/null
lsmod | grep -E 'ili9341|fb_sys_fops|syscopyarea|sysfillrect|sysimgblt'

/tmp/fb_test /dev/fb1
evtest /dev/input/event0
QT_QPA_PLATFORM=linuxfb QT_QPA_FB=/dev/fb1 /tmp/system-monitor
```

Pass conditions:

- `/dev/fb1` renders correct red, green, and blue through `fb_test`.
- Setting `dtoverlay=ili9341-xpt2046-overlay,fps=20` reports `fps=20` in the
  display-driver probe log after reboot.
- The XPT2046 device reports stable taps and drags without duplicate backends.
- The app persists settings and responds through the intended touchscreen.
- Wi-Fi reconnects after two reboots.

## Common Problems

| Symptom | Cause to check first | Action |
| --- | --- | --- |
| No `/dev/fb1` | Overlay, SPI binding, or display module load | Inspect `dmesg`, boot `config.txt`, and `lsmod`. |
| `Unknown symbol fb_sys_write` | Module dependency metadata not refreshed | Install under `/lib/modules/$(uname -r)/extra`, run `depmod -a`, then `modprobe`. |
| Red and blue reversed | Panel color-order setup | Run `fb_test`; keep RGB565 framebuffer and verify MADCTL BGR is set. |
| No input event | Overlay IRQ/GPIO or touch module bind | Inspect `dmesg` and `/dev/input/event*`. |
| Touch bounce or jumps | PENIRQ noise, calibration, or duplicate Qt backend | Compare `evtest` first, then check the single-backend startup log. |
| No `wlan0` | Broadcom module or firmware missing | Check `dmesg` for `brcmfmac` and image manifest. |
| Wi-Fi has no address | WPA config or DHCP service failure | Check file mode `0600`, `wpa_supplicant@wlan0`, and `dhcpcd@wlan0`. |
| Qt library missing | Image runtime and SDK do not match | Rebuild the runtime image and cross-build from the matching SDK. |

## Pseudo Inode Mismatch

A Pseudo inode mismatch is host build-state corruption, not a driver or Qt
source failure. Stop BitBake tasks and clean the failed recipe first:

```sh
bitbake -c cleansstate <failed-recipe>
bitbake <failed-recipe>
```

If it persists, remove only the affected `build/tmp` after BitBake stops.
Preserve `downloads` and `sstate-cache` unless they are known to be corrupt.
