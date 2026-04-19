FILESEXTRAPATHS:prepend := "${THISDIR}/files:"

# Add kernel patch and config fragment
SRC_URI += " \
    file://0001-fbdev-export-fb_deferred_io_mmap.patch \
    file://defio.cfg \
"
