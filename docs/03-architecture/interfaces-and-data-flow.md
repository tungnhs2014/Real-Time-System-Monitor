# Interfaces And Data Flow

## Purpose

This document connects the architecture views to concrete interface and data-flow boundaries in the current brownfield implementation and near-term target.

## Runtime Interface Boundaries

### QML to backend interface

Current interface:
- `main.cpp` registers `systemInfo` and `navigation` as QML context properties
- QML pages bind primarily to `systemInfo`

Key current contract:
- QML reads runtime state from `SystemController` properties
- QML invokes actions such as `saveSettings()`, `reboot()`, and `shutdown()` through `SystemController`

### Controller to model interface

Current interface:
- `SystemController` directly owns monitor/service objects
- `SystemController` calls monitor methods on refresh
- `SystemController` forwards settings and action calls to `SettingsManager`

Current implication:
- controller-to-model boundaries are explicit in code, but orchestration is centralized and broad

### Model to platform interface

Current interface sources:
- CPU: `/proc/stat`, thermal sensor paths, CPU frequency paths, `/proc/loadavg`
- GPU: `vcgencmd` plus fallback sensor paths
- Memory: `/proc/meminfo`
- Storage: `statvfs`, `/proc/meminfo`, `/sys/block/*/stat`
- Network: `/proc/net/dev`, `/proc/net/tcp`, `/proc/net/tcp6`, `/sys/class/net`, `ip addr show`
- Settings/system info: `QSettings`, `/etc/hostname`, `/etc/os-release`, `uname -r`, `reboot`, `shutdown -h now`

## Current Data Flows

### Flow 1: App startup

1. `main.cpp` configures the Qt platform and target-specific environment.
2. The app constructs `SystemController` and `NavigationController`.
3. `SystemController` constructs monitor/services and loads persisted settings.
4. `QQmlApplicationEngine` loads `Main.qml`.
5. QML pages bind against `systemInfo`.

### Flow 2: Periodic refresh

1. `SystemController` timer fires.
2. Monitor objects read Linux runtime data.
3. Parsed values are converted into controller-owned QML-facing properties.
4. QML bindings refresh the visible UI.

### Flow 3: Settings persistence

1. The settings page changes threshold values through `systemInfo`.
2. `SystemController` forwards the new values into `SettingsManager`.
3. The user triggers `saveSettings()`.
4. `SettingsManager` writes persisted values through `QSettings`.

### Flow 4: Threshold logging

1. Runtime refresh updates CPU and RAM usage values.
2. `SystemController` compares current values against configured thresholds.
3. When warning state changes, `SettingsManager` appends a log entry.
4. The settings log view renders the latest entries through `systemInfo.systemLogs`.

### Flow 5: System action handoff

1. The settings page shows a confirmation dialog.
2. On confirmation, QML invokes `systemInfo.reboot()` or `systemInfo.shutdown()`.
3. `SystemController` forwards the call to `SettingsManager`.
4. `SettingsManager` logs the action and launches the corresponding system command.

## BSP And Deployment Data Flow

### Display path

1. The Yocto BSP layer delivers the overlay and display module.
2. The overlay binds the ILI9341 device on SPI0.0.
3. The display driver provides framebuffer output.
4. Qt renders through the Linux framebuffer target path on Raspberry Pi.

### Touch path

1. The Yocto BSP layer delivers the overlay and touch module.
2. The overlay binds the XPT2046 device on SPI0.1.
3. The touch driver reports Linux input events.
4. Qt receives touch input through the evdev input path.

## Target Interface Direction

- keep monitor/platform interfaces in C++ rather than pushing them into QML
- narrow controller responsibilities over time by introducing clearer page-level ownership
- keep product packaging interfaces in `meta-system-monitor/` instead of mixing them back into BSP recipes

## Current Risks In Interface Design

- some backend-exposed properties imply broader settings behavior than the current QML page actually completes
- `NavigationController` is available in the runtime surface, but current QML flow still bypasses it for primary page navigation
- diagrams can easily drift unless they distinguish current versus target behavior
