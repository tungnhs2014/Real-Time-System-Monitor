SUMMARY = "Raspberry Pi ILI9341 and XPT2046 device tree overlay"
LICENSE = "GPL-2.0-only"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/GPL-2.0-only;md5=801f80980d171dd6425610833a22dbe6"

SRC_URI = "file://rpi-ili9341-xpt2046-overlay.dts"

S = "${WORKDIR}"

DEPENDS = "dtc-native"

do_compile() {
    dtc -@ -I dts -O dtb -o rpi-ili9341-xpt2046-overlay.dtbo rpi-ili9341-xpt2046-overlay.dts
}

do_install() {
    install -d ${D}/boot/overlays
    install -m 0644 rpi-ili9341-xpt2046-overlay.dtbo ${D}/boot/overlays/
}

FILES:${PN} = "/boot/overlays/rpi-ili9341-xpt2046-overlay.dtbo"
