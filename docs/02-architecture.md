# Architecture

## Repository Zones

- `app/`: Qt/QML application and desktop-oriented CMake build.
- `yocto/meta-rpi-ili9341-bsp/`: hardware-facing BSP layer for overlay, display/touch modules, and kernel support.
- `yocto/meta-system-monitor/`: product-side app recipe, systemd service, and image recipe.
- `docs/`: compact source-of-truth project documentation.

## Runtime Shape

The app uses a practical layered architecture:

- QML renders state and emits user actions.
- `SystemController` is the QML-facing facade exposed as `systemInfo`.
- `SystemMetricsService` owns monitor objects and returns `SystemMetricsSnapshot` DTOs.
- Monitor classes read Linux runtime sources such as `/proc`, `/sys`, filesystem metadata, and platform commands.
- `SettingsManager` owns persisted settings, system info, logs, and system action handoff.
- `WarningThresholdPolicy` owns pure CPU/RAM threshold decisions.

This is not pure MVC and not a full MVVM implementation. `SystemController` remains intentionally broad in the current codebase so the existing QML API stays stable.

## Important Interfaces

- QML binds to `systemInfo` properties and invokes `saveSettings()`, `reboot()`, and `shutdown()`.
- `SystemController` periodically asks `SystemMetricsService` for a complete snapshot.
- `SystemMetricsService` pulls values from CPU, GPU, memory, storage, and network monitors.
- `SettingsManager` persists values with `QSettings` and launches system commands for reboot/shutdown.

## Main Data Flows

Startup:
1. `main.cpp` configures Qt platform behavior and constructs `SystemController`.
2. `SystemController` creates services, loads settings, performs an initial refresh, and starts its timer.
3. `QQmlApplicationEngine` loads `qrc:/qml/Main.qml`.
4. QML pages bind to `systemInfo`.

Periodic refresh:
1. `SystemController` timer fires.
2. `SystemMetricsService` collects monitor values into `SystemMetricsSnapshot`.
3. `SystemController` applies values and emits property notifications.
4. QML redraws bound controls.

Settings:
1. QML updates supported threshold properties.
2. `SystemController` forwards values to `SettingsManager`.
3. `saveSettings()` writes values through `QSettings`.

## Design Rules

- Keep QML lightweight: presentation and user actions only.
- Keep Linux reads and commands in C++ platform/model code.
- Preserve QML-facing property names unless doing a deliberate migration.
- Add abstractions only when they reduce real complexity or support tests.
- Keep product packaging separate from the BSP layer.

## Known Architecture Debt

- `SystemController` has many QML-facing properties and responsibilities.
- Platform I/O is still mostly inside monitor classes rather than separate adapter interfaces.
- Automated tests are not broad enough to support a risky full runtime rewrite.
