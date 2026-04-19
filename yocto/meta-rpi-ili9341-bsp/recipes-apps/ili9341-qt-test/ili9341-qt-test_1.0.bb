SUMMARY = "ILI9341 Qt6 QML Test"
LICENSE = "GPL-2.0-only"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/GPL-2.0-only;md5=801f80980d171dd6425610833a22dbe6"

inherit qt6-qmake

SRC_URI = "file://main.cpp \
           file://main.qml \
           file://qml.qrc \
           file://ili9341-qt-test.pro"

S = "${WORKDIR}"

DEPENDS = "qtbase qtdeclarative"

do_install() {
    install -d ${D}${bindir}
    install -m 0755 ili9341-qt-test ${D}${bindir}/
}
