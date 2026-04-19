# Start Here

This documentation is intentionally compact. Current source code and Yocto metadata are the source of truth; docs describe what exists, what is pending validation, and how to build or test it.

## Read In This Order
1. `docs/01-product.md` - product scope, current features, known gaps.
2. `docs/02-architecture.md` - repository zones, runtime layers, data flow.
3. `docs/diagrams.md` - focused diagrams for review and debugging.
4. `docs/03-bsp-yocto.md` - display, touch, device tree, and Yocto layers.
5. `docs/04-build-run-test.md` - local checks, Yocto checks, board checklist.
6. `docs/05-troubleshooting.md` - common app, BSP, touch, and packaging issues.

## Repository Map
- `app/`: Qt/QML userspace application.
- `yocto/meta-rpi-ili9341-bsp/`: hardware-facing BSP layer for overlay, display driver, touch driver, and kernel support.
- `yocto/meta-system-monitor/`: product-side app package, systemd service, and image recipe.
- `docs/`: compact product, architecture, BSP, verification, and troubleshooting docs.

## Important Reality Checks
- "Real-Time" means near-real-time periodic monitoring UI, not hard real-time scheduling.
- `SystemController` is still the broad QML-facing facade; it has not been replaced by a full MVVM rewrite.
- Automated tests are currently light; hardware display/touch validation remains board-only.
- Product-side Yocto packaging exists, but release confidence requires `bitbake` and board validation.
