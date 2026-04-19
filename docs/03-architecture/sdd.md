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
- CPU monitoring
- GPU summary monitoring
- memory monitoring
- storage monitoring
- network monitoring
- settings persistence

### `app/src/controller/`
Responsibilities:
- aggregate backend state for QML
- own refresh coordination
- expose QML-facing properties

### `app/qml/`
Responsibilities:
- shell and page flow
- reusable components
- UI interaction

## State Ownership
- monitor state originates in model classes
- aggregated UI-facing state is exposed through `SystemController`
- settings persistence is owned by `SettingsManager`

## Runtime Flows
- startup flow: bootstrap Qt, register/expose controllers, load QML
- refresh flow: collect metrics, update controller state, redraw QML bindings
- settings flow: UI change, controller handoff, persistence update

## BSP Integration Responsibility Split
- overlay ownership: hardware description and SPI/GPIO binding
- display driver ownership: framebuffer-backed display output over SPI
- touch driver ownership: Linux input events from resistive touch controller
- Yocto layer ownership: integrate recipes, config, and packaging paths

## Fallback And Error Behavior
- missing metrics should degrade specific readings, not crash the whole UI
- incomplete target-state packaging should be documented as partial, not presented as finished
