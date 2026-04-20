# Migration Strategy

## Purpose

This repository is a brownfield embedded product repository. The migration strategy exists to make repository normalization and architecture cleanup reviewable without rewriting history.

This file separates:
- the current implemented baseline
- the transition work already selected
- later ideas that are still optional

## Current Baseline

Current repository truth:
- the userspace monitor application already runs from `app/`
- the active display and touch BSP path already lives in `yocto/meta-rpi-ili9341-bsp/`
- the product-side Yocto layer `yocto/meta-system-monitor/` exists only as scaffold
- the public documentation set is present but still being strengthened for brownfield review quality

Current runtime truth:
- `SystemController` is the main aggregation facade for QML-facing state
- monitor logic is split across `CpuMonitor`, `GpuMonitor`, `MemoryMonitor`, `StorageMonitor`, `NetworkMonitor`, and `SettingsManager`
- the main page flow is currently handled directly in `app/qml/Main.qml` through `StackView`
- `NavigationController` is registered in QML context, but it is not the primary navigation path in the current UI implementation

## Migration Goals

- preserve working application and BSP behavior while documentation quality improves
- make repo zones and architecture boundaries reviewable
- document `as-is`, `transition`, and `target` views separately
- keep future runtime refactoring incremental rather than disruptive
- avoid renaming or restructuring decisions that break naming consistency across recipes, overlays, modules, and docs

## Migration Phases

### Phase 1: Documentation normalization

Status:
- In progress

Objective:
- make the public docs match current source and current repository layout

Key work:
- strengthen requirements and use cases
- add missing brownfield architecture views
- correct diagrams that do not match current runtime behavior
- keep README as a landing page only

Completion signal:
- reviewers can identify current implementation truth without reading source first

### Phase 2: Verification normalization

Status:
- Target

Objective:
- link requirements, architecture, and validation into one reviewable verification path

Key work:
- add requirement traceability
- define test cases and bring-up checks more explicitly
- state clearly what remains manual versus automated

Completion signal:
- each important requirement has an associated validation path

### Phase 3: Product packaging completion

Status:
- Target

Objective:
- complete the product-side Yocto packaging path for the main application

Key work:
- move from scaffold-only `meta-system-monitor/` to a reviewable app packaging path
- keep BSP responsibilities separate from product packaging responsibilities

Completion signal:
- the main monitor app has a documented and reviewable home in the product layer

### Phase 4: Runtime modularization refinement

Status:
- Optional target

Objective:
- reduce current runtime coupling without destabilizing the working app

Key work:
- tighten page-level ownership over time
- decide whether `NavigationController` should become a real primary runtime path or remain secondary
- document incremental runtime refactoring slices before any implementation work

Completion signal:
- runtime ownership becomes clearer without a full rewrite

## Explicit Non-Goals For This Migration

- splitting the repository into multiple repositories
- replacing Qt/QML
- replacing Yocto
- introducing a full runtime redesign before documentation and packaging become reviewable
- documenting future ideas from `Project.txt` as if they were current or committed target scope

## Risks To Control During Migration

- mixing current implementation and target design in the same statement
- letting diagrams drift away from the current QML navigation implementation
- overstating the maturity of product-layer packaging
- treating backend-exposed properties as completed UI workflows when the page implementation is still partial

## Decision Rule

When current source and older prose disagree, current source wins for `as-is` documentation.

When original project intent contains broader scope than the current repo implements, the extra scope must stay in `future ideas` until it is explicitly adopted as target scope.
