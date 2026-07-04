# Optional SDK tasks. Copy to poky/build/conf/sdk.inc, then add one line to
# local.conf: require sdk.inc

SDKMACHINE ?= "x86_64"

TOOLCHAIN_HOST_TASK:append = " \
    nativesdk-cmake \
    nativesdk-ninja \
    nativesdk-pkgconfig \
    nativesdk-packagegroup-qt6-toolchain-host-essentials \
"

TOOLCHAIN_TARGET_TASK:append = " \
    qtbase-dev \
    qtdeclarative-dev \
"
