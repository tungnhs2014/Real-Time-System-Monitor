# As-Is Architecture

## Purpose

This document describes the current implemented architecture of the repository and runtime system.

It is not the target design document.

## Repository Zones

The current repository is already organized into explicit zones:
- `app/`: Qt/QML userspace application
- `yocto/meta-rpi-ili9341-bsp/`: active BSP layer for display, touch, overlay, and kernel integration assets
- `yocto/meta-system-monitor/`: product-side Yocto layer scaffold
- `docs/`: public design and verification docs
- `private-docs/ai/`: internal AI workflow rules

## Current Runtime Composition

### Application shell

Current implementation:
- the app boots in `app/src/main.cpp`
- `QQmlApplicationEngine` loads `qrc:/qml/Main.qml`
- `systemInfo` and `navigation` are both registered as context properties

Important current-state note:
- `navigation` is registered, but the actual page flow in the current UI is handled primarily inside `app/qml/Main.qml` through `StackView`, page components, and page-emitted signals

### Controller layer

Current implementation:
- `SystemController` is the main QML-facing aggregation facade
- it owns the update timer, pulls values from the monitor classes, and exposes properties used across the QML pages
- it also forwards settings persistence and system-action calls into `SettingsManager`

Current limitation:
- `SystemController` is broad and mixes metric aggregation, settings coordination, threshold checking, and user action handoff

### Model layer

Current implementation:
- `CpuMonitor` reads CPU usage, per-core usage, frequency, load average, and temperature sensor data
- `GpuMonitor` reads GPU temperature and GPU memory where `vcgencmd` is available, with fallbacks when it is not
- `MemoryMonitor` reads RAM values from `/proc/meminfo`
- `StorageMonitor` reads filesystem usage through `statvfs`, swap from `/proc/meminfo`, and block I/O stats from `/sys/block`
- `NetworkMonitor` detects an active interface and reads traffic, addressing, and connection data from Linux runtime sources
- `SettingsManager` owns persisted settings, system info parsing, runtime logs, and reboot/shutdown command launching

### QML page layer

Current implementation:
- `Dashboard.qml` is the initial page and summary view
- `CpuDetail.qml`, `MemoryDetail.qml`, `StorageDetail.qml`, and `NetworkDetail.qml` provide separate detail pages
- `Settings.qml` provides system info, threshold editing, logs, and reboot/shutdown confirmation flows

Important current-state note:
- there is no dedicated GPU detail page
- the dashboard contains a GPU summary card, but its click path currently routes to the network detail page rather than to a GPU detail page

## Current Data Flow

### Startup flow

1. `main.cpp` configures the Qt platform environment.
2. The app constructs `SystemController` and `NavigationController`.
3. QML loads `Main.qml`.
4. `SystemController` initializes monitor objects, loads settings, performs an initial refresh, and starts a periodic timer.

### Refresh flow

1. `SystemController` refreshes each monitor.
2. Monitor classes read from Linux runtime sources such as `/proc`, `/sys`, filesystem metadata, and shell commands.
3. `SystemController` updates QML-facing properties.
4. Bound QML components redraw.

### Settings and actions flow

1. QML writes changed threshold values into `systemInfo`.
2. `SystemController` forwards those changes into `SettingsManager`.
3. The save action persists settings through `QSettings`.
4. Reboot and shutdown actions are confirmed in QML and then launched by `SettingsManager` through detached system commands.

## Current BSP And Integration Architecture

Current implementation:
- the Raspberry Pi overlay lives in `recipes-bsp/rpi-ili9341-xpt2046-dtbo/`
- the framebuffer-style display driver lives in `recipes-kernel/ili9341-fb/`
- the touch driver lives in `recipes-kernel/xpt2046-touch/`
- Linux patch and config support live in `recipes-kernel/linux/`
- the BSP layer also contains helper recipes under `recipes-apps/` that support bring-up and validation

Important current-state note:
- the product-side layer is not yet the primary packaging path for the app

## Current Deployment Paths

Desktop-oriented path:
- configure from the repository root
- build the app through `app/CMakeLists.txt`
- run the Qt/QML app in a desktop environment for iteration

Target-oriented path:
- integrate the BSP layer through Yocto
- build the overlay and kernel modules
- bring up framebuffer and touch input on target hardware
- complete product-side app packaging later

## Current Known Gaps

- no dedicated GPU detail page exists
- the `NavigationController` is present but not the primary navigation mechanism in the current UI
- the settings UI does not yet provide a completed interaction path for every persisted backend setting
- product-side Yocto packaging remains scaffolded rather than finished
- automated tests are not yet committed
