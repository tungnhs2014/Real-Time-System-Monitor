# Selected Solution

## Chosen Direction
Use a **single product monorepo with explicit zones**:
- `app/` for the userspace application
- `yocto/meta-rpi-ili9341-bsp/` for active BSP integration
- `yocto/meta-system-monitor/` for product-side packaging
- `docs/` for public design and verification material
- `private-docs/ai/` for internal AI workflow rules

## Scope Of The Selected Solution
This selected solution resolves the repository, documentation, and layer-boundary design problem first.

It does not claim that the runtime application internals have already been fully redesigned.

## Why This Was Chosen
- keeps one-repo operational simplicity
- aligns better with Yocto layer conventions
- avoids calling a full BSP layer merely `drivers`
- supports design-led documentation without bloating README
- allows the runtime app to remain stable while future refactoring moves gradually toward clearer page-level ownership

## Current State
- userspace app is implemented and now lives under `app/`
- active BSP integration is implemented and now lives under `yocto/meta-rpi-ili9341-bsp/`
- product-side Yocto layer exists as scaffold only
- public docs are being normalized around design workflow
- `SystemController` is still the primary runtime aggregation facade
- page flow is still driven mainly from `app/qml/Main.qml`
- `NavigationController` exists but is not yet the main navigation path

## Transition State

This brownfield effort is intentionally using a transition architecture rather than pretending the target state already exists.

Transition rules:
- preserve working runtime and BSP behavior while documentation is corrected
- add `as-is` and migration artifacts before broad refactoring
- keep the repo split between app, BSP, product layer, docs, and private AI rules stable
- correct diagrams and architecture docs so they describe the current source honestly

## Target State
- README is short and routes readers into `docs/`
- docs follow the sequence: context, requirements, options, decision, architecture, verification
- diagrams are versioned as text files
- ADRs capture major decisions instead of burying them in prose
- runtime architecture can evolve incrementally instead of requiring a disruptive rewrite first
- product-side packaging can mature without collapsing BSP and product concerns back together

## Migration Consequences
- old path names should no longer be used in docs
- review and onboarding should start from `docs/00-start-here.md`
- current implementation and future phases must be documented separately
- backend-exposed properties must not be mistaken for fully completed UI workflows unless the current pages actually implement them
