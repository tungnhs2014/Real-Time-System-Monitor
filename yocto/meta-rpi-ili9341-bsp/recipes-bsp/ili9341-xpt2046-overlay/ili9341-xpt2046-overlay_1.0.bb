SUMMARY = "Device tree overlay for ILI9341 display and XPT2046 touch"
LICENSE = "GPL-2.0-only"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/GPL-2.0-only;md5=801f80980d171dd6425610833a22dbe6"

SRC_URI = "file://ili9341-xpt2046-overlay.dts"

S = "${WORKDIR}"

DEPENDS = "dtc-native"

do_compile() {
    dtc -@ -I dts -O dtb -o ili9341-xpt2046-overlay.dtbo ili9341-xpt2046-overlay.dts
}

do_install() {
    install -d ${D}/boot/overlays
    install -m 0644 ili9341-xpt2046-overlay.dtbo ${D}/boot/overlays/
}

FILES:${PN} = "/boot/overlays/ili9341-xpt2046-overlay.dtbo"
