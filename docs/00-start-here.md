# Start Here

This project has two development paths.

| Path | Purpose | Status |
| --- | --- | --- |
| Desktop Qt build | Iterate on C++ and QML without hardware. | Local-only |
| Manual Raspberry Pi bring-up | Build a base image, then deploy BSP and app artifacts through SCP. | Board-only |

Read in this order:

1. [Application Scope](01-product.md)
2. [Architecture](02-architecture.md)
3. [Hardware and BSP](03-hardware-bsp.md)
4. [Yocto Build and Deploy](04-yocto-build-deploy.md)
5. [Verification and Troubleshooting](05-verification-troubleshooting.md)
6. [Diagrams](diagrams.md)

The only tracked Yocto configuration is
`yocto/config/rpi3-ili9341-bringup/`. Copy `local.conf` into the local Poky
build directory and register upstream/BSP layers with `bitbake-layers`. The
repository does not build a packaged auto-start image.
