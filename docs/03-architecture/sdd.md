# Software Design Document (SDD)

## Scope
This SDD focuses on the application and repository module design that implements the selected architecture direction.

## Application Modules

### `app/src/common/`
Responsibilities:
- shared utilities
- logging
- file reading helpers

### `app/src/model/`
Responsibilities:
- CPU monitoring from `/proc` and temperature/frequency sensor paths
- GPU summary monitoring with `vcgencmd`-based and fallback behavior
- memory monitoring from `/proc/meminfo`
- storage, swap, and block I/O monitoring
- network interface detection and traffic monitoring
- settings persistence, system info parsing, runtime logs, and reboot/shutdown command handoff

### `app/src/controller/`
Responsibilities:
- aggregate backend state for QML
- own refresh coordination
- expose QML-facing properties
- forward settings persistence and system actions to `SettingsManager`
- perform threshold-based warning checks for CPU and RAM

### `app/qml/`
Responsibilities:
- shell and page flow
- reusable components
- UI interaction

Important current-state note:
- current page flow is implemented primarily in `Main.qml` through `StackView` and page-emitted signals
- `NavigationController` is available in QML context but is not currently the main runtime navigation path

## State Ownership
- monitor state originates in model classes
- aggregated UI-facing state is exposed through `SystemController`
- settings persistence is owned by `SettingsManager`
- page stack state currently lives mainly in QML rather than in a fully centralized navigation controller

## Runtime Flows
- startup flow: bootstrap Qt, register/expose controllers, load QML
- refresh flow: collect metrics, update controller state, redraw QML bindings
- settings flow: UI change, controller handoff, persistence update
- system action flow: QML confirm dialog, controller forward, `SettingsManager` command launch
- threshold alert flow: runtime metric update, threshold comparison, log append when warning level changes

## BSP Integration Responsibility Split
- overlay ownership: hardware description and SPI/GPIO binding
- display driver ownership: framebuffer-backed display output over SPI
- touch driver ownership: Linux input events from resistive touch controller
- Yocto layer ownership: integrate recipes, config, and packaging paths

## Fallback And Error Behavior
- missing metrics should degrade specific readings, not crash the whole UI
- incomplete target-state packaging should be documented as partial, not presented as finished
- unavailable GPU metrics may fall back to thermal-zone or unavailable values rather than a complete page failure
- network, storage, and sensor readers use fallback defaults such as `0`, `N/A`, or `-1` when sources are missing
