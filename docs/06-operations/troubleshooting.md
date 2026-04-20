# Troubleshooting

## Purpose

This document captures current troubleshooting guidance that should not bloat README.

## Documentation-Level Troubleshooting

### The docs appear to overclaim the current implementation

Check:
- `docs/03-architecture/as-is-architecture.md`
- `docs/02-solution/migration-strategy.md`
- `docs/05-verification/known-limitations.md`

Rule:
- current source wins for `as-is` documentation

## App-Level Troubleshooting

### GPU detail seems to be missing

Current reality:
- this is expected
- the current app has a GPU summary on the dashboard, but no dedicated GPU detail page

### Navigation behavior looks different from older diagrams or prose

Current reality:
- the current main page flow is still driven largely by `Main.qml` and `StackView`
- `NavigationController` is present but not the primary navigation path

### Some settings appear in backend state but not fully in the current UI

Current reality:
- this is expected during the current brownfield state
- threshold editing is the clearest completed settings-edit path
- other persisted settings should not be treated as fully surfaced UI workflows unless the page implements them directly

## BSP-Level Troubleshooting

### The product layer looks incomplete

Current reality:
- this is expected
- `meta-system-monitor/` is intentionally still a scaffold

### The Yocto area looks like more than just drivers

Current reality:
- this is correct
- the active Yocto content includes layer metadata, recipes, overlay, kernel modules, and helper assets, not only driver sources
