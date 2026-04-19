# Product

## Summary

System Monitor is an embedded monitoring UI for Raspberry Pi-class boards with an ILI9341 SPI display and XPT2046 resistive touch controller. The product combines a Qt/QML userspace app, Linux display/touch integration, and Yocto packaging.

"Real-Time" in this repository means near-real-time periodic UI updates. It does not claim hard real-time latency or deterministic scheduling.

## Current Features

- Dashboard summary for CPU, GPU summary, memory, storage, and network values.
- CPU detail page with temperature history and per-core usage.
- Memory detail page with RAM usage, used/free values, and cache visibility.
- Storage detail page with root filesystem, swap, and I/O trend values.
- Network detail page with active interface, IP, MAC, speeds, packet rate, and active connections.
- Settings page with system information, threshold editing, logs, and guarded reboot/shutdown actions.
- Persistent settings through `QSettings`.
- Yocto BSP assets for ILI9341 display, XPT2046 touch, overlay, and kernel support.
- Product-side Yocto layer for the app recipe, systemd service, and image composition.

## Current Gaps

- No dedicated GPU detail page exists.
- The settings UI does not expose every persisted backend setting as a complete interaction flow.
- Automated tests are limited; most UI, BSP, and Yocto checks are manual or environment-dependent.
- Product-side Yocto packaging still needs target Yocto and board validation before it should be treated as release-proven.

## Out Of Current Scope

These ideas are present in the original project intent but are not current implemented scope:

- top-process views for CPU or RAM consumers
- service status pages
- remote monitoring API
- firmware update over network
- backup and restore workflows
- integration with external monitoring systems

## Requirements Snapshot

| Area | Current acceptance |
| --- | --- |
| UI | App starts and renders the dashboard at 320x240. |
| Metrics | CPU, memory, storage, network, and GPU summary values degrade safely when sources are missing. |
| Settings | Supported threshold values save and reload. |
| Actions | Reboot and shutdown are gated by confirmation dialogs. |
| BSP | Overlay and module recipes exist for display and touch bring-up. |
| Packaging | App, service, and image recipes exist in `meta-system-monitor/`, pending target validation. |
