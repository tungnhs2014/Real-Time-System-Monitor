# Requirements

This file separates:
- current implemented requirements verified from source under `app/` and `yocto/`
- target requirements already chosen for this brownfield normalization effort
- future ideas that appear in project intent material but are not committed implementation facts

It must not present planned or aspirational features as current behavior.

## Current Implemented Functional Requirements

| ID | Requirement | Priority | Status | Current acceptance baseline | Verification anchor |
|----|-------------|----------|--------|-----------------------------|---------------------|
| FR-01 | Show a dashboard summary on the 320x240 UI with CPU, GPU summary, memory, storage, and network snapshot cards | High | Current | Dashboard loads as the initial page and renders summary values on the current Qt/QML shell | QML startup and dashboard rendering |
| FR-02 | Show CPU monitoring detail with temperature history and per-core usage | High | Current | CPU detail page renders a line chart from `tempHistory` and per-core bars from `coreUsages` | App page navigation and CPU detail smoke check |
| FR-03 | Show memory detail with RAM usage, used/free values, and cache visibility | High | Current | Memory detail page renders RAM usage and summary values sourced from `/proc/meminfo` | App page navigation and memory detail smoke check |
| FR-04 | Show storage detail with root partition usage, swap usage, and I/O trend data | High | Current | Storage detail page renders root partition, swap, read rate, write rate, and I/O history | App page navigation and storage detail smoke check |
| FR-05 | Show network detail with active interface, IP, MAC, upload/download history, packet rate, and active connection count | High | Current | Network detail page renders interface metadata plus current and historical traffic values | App page navigation and network detail smoke check |
| FR-06 | Show system information, threshold editing, recent logs, and system actions in the settings area | High | Current | Settings page renders system info, editable CPU/RAM thresholds, log list, and reboot/shutdown confirmation dialogs | Settings page walkthrough |
| FR-07 | Persist supported settings through `QSettings` when the user saves changes | High | Current | Backend persists update interval, dark mode, sound alert, and warning thresholds even though the current UI only exposes threshold editing directly | Settings save/reload check |
| FR-08 | Produce warning log entries when CPU or RAM usage crosses configured thresholds | Medium | Current | Threshold changes update state, and runtime threshold checks append warning or critical log entries | Threshold behavior review |
| FR-09 | Bring up ILI9341 display output and XPT2046 touch input on the Raspberry Pi target through the Yocto BSP layer | High | Current | Overlay, display module, and touch module exist in the BSP layer and are intended to load on target hardware | Overlay/module build and target bring-up |

## Current Limitations That Must Stay Explicit

| ID | Limitation | Why it matters |
|----|------------|----------------|
| LIM-01 | There is no dedicated GPU detail page | The dashboard includes GPU summary values, but navigation does not lead to a GPU detail view |
| LIM-02 | The current settings UI does not provide a completed control path for every persisted setting | `updateInterval`, `darkMode`, and `soundAlert` exist in backend state, but the page currently exposes threshold editing clearly and only shows a placeholder interaction for update interval |
| LIM-03 | Product-side Yocto packaging for the main monitor application is still a scaffold | Reviewers must not treat `meta-system-monitor/` as a completed delivery path |

## Target Functional And Delivery Requirements

These requirements are part of the current documentation and repository normalization effort, but they are not all fully implemented yet.

| ID | Requirement | Priority | Status | Target outcome |
|----|-------------|----------|--------|----------------|
| TR-01 | Keep README as a concise landing page and move detailed design into `docs/` | High | In progress | README stays short; deep design and verification live in versioned docs |
| TR-02 | Separate current implementation, target design, and future ideas across the documentation set | High | In progress | Brownfield readers can distinguish as-is truth from to-be direction without inference |
| TR-03 | Keep app, BSP, and product packaging responsibilities separated in both repo structure and docs | High | In progress | `app/`, `yocto/meta-rpi-ili9341-bsp/`, and `yocto/meta-system-monitor/` remain conceptually distinct |
| TR-04 | Complete a reviewable product-side Yocto packaging path for the main app in `meta-system-monitor/` | Medium | Target | Product packaging becomes more than a scaffold |
| TR-05 | Maintain traceable architecture, decision, and verification artifacts as text-first repository assets | High | In progress | Requirements, diagrams, ADRs, and verification notes are reviewable in git |

## Future Ideas From Original Project Intent

These items appear in project intent material such as `Project.txt`, but they are not current implementation requirements and are not yet committed target requirements for the brownfield cleanup.

- top-process views for CPU or RAM consumers
- service status visibility
- remote API access
- backup and restore flows
- network firmware update flows
- cross-system monitoring integration

## Non-Functional Requirements

| ID | Requirement | Status | Notes |
|----|-------------|--------|-------|
| NFR-01 | Keep the UI usable at 320x240 resolution | Current | Current QML pages are explicitly sized for 320x240 |
| NFR-02 | Keep the app responsive on Raspberry Pi-class hardware | Current baseline | The app uses a lightweight Qt/QML stack and periodic polling design |
| NFR-03 | Degrade specific readings instead of crashing the whole UI when some metric sources are unavailable | Current baseline | Many monitor classes return `0`, `N/A`, or `-1` fallbacks |
| NFR-04 | Support both desktop-oriented application validation and target-hardware bring-up | Current | Root CMake path and Yocto/BSP path both exist |
| NFR-05 | Keep documentation and diagrams versionable as text in git | Current target | Mermaid and Markdown are the intended documentation media |
| NFR-06 | Keep repo zones explicit for app, BSP, product layer, docs, and private AI rules | Current target | This is part of the selected repository design |
| NFR-07 | Keep the documentation reviewable against current brownfield truth | Current target | Docs must map to source, not only to intent material |

## Out Of Scope For The Current Brownfield Cleanup

- redesigning the product into multiple repositories
- replacing Qt/QML with another UI stack
- replacing Yocto with a different build system
- claiming complete automated test coverage before tests actually exist
- documenting `Project.txt` future ideas as if they were shipped product scope

## Current Gaps Against Target Requirements

- `meta-system-monitor/` is scaffolded and does not yet package the main Qt/QML app
- brownfield transition artifacts are still being strengthened so that `as-is`, `transition`, and `target` views stay separate
- verification traceability from requirements to checks is still incomplete
- some architecture diagrams still need correction to match the current navigation/runtime implementation exactly
