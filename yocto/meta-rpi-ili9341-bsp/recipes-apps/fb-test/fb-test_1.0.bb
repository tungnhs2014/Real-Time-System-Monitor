SUMMARY = "Framebuffer Test Utility for ILI9341"
DESCRIPTION = "Simple test program to verify ILI9341 framebuffer functionality"
LICENSE = "GPL-2.0-only"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/GPL-2.0-only;md5=801f80980d171dd6425610833a22dbe6"

SRC_URI = "file://fb_test.c"

S = "${WORKDIR}"

do_compile() {
    ${CC} ${CFLAGS} ${LDFLAGS} -o fb_test fb_test.c
}

do_install() {
    install -d ${D}${bindir}
    install -m 0755 fb_test ${D}${bindir}/
}

