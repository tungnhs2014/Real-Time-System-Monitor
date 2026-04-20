# Requirement Traceability

## Purpose

This document maps important current and target requirements to their design and verification anchors.

It is intentionally lightweight for the brownfield repository, but it must stay explicit enough to support review.

| Requirement | Source/design anchor | Current implementation anchor | Verification anchor | Status |
|------------|----------------------|-------------------------------|---------------------|--------|
| FR-01 Dashboard summary | `01-product/requirements.md`, `03-architecture/as-is-architecture.md` | `app/qml/views/pages/Dashboard.qml` | App startup and dashboard smoke check | Current |
| FR-02 CPU detail | `01-product/use-cases.md`, `03-architecture/sdd.md` | `app/qml/views/pages/CpuDetail.qml`, `app/src/model/CpuMonitor.*` | CPU detail smoke check | Current |
| FR-03 Memory detail | `01-product/use-cases.md`, `03-architecture/sdd.md` | `app/qml/views/pages/MemoryDetail.qml`, `app/src/model/MemoryMonitor.*` | Memory detail smoke check | Current |
| FR-04 Storage detail | `01-product/use-cases.md`, `03-architecture/sdd.md` | `app/qml/views/pages/StorageDetail.qml`, `app/src/model/StorageMonitor.*` | Storage detail smoke check | Current |
| FR-05 Network detail | `01-product/use-cases.md`, `03-architecture/sdd.md` | `app/qml/views/pages/NetworkDetail.qml`, `app/src/model/NetworkMonitor.*` | Network detail smoke check | Current |
| FR-06 Settings info/logs/actions | `01-product/use-cases.md`, `03-architecture/interfaces-and-data-flow.md` | `app/qml/views/pages/Settings.qml`, `app/src/model/SettingsManager.*` | Settings walkthrough | Current |
| FR-07 Settings persistence | `01-product/requirements.md`, `03-architecture/interfaces-and-data-flow.md` | `SettingsManager::save/load`, `SystemController::saveSettings` | Save/reload settings check | Current |
| FR-08 Threshold-based log events | `01-product/requirements.md`, `03-architecture/sdd.md` | `SystemController::checkThresholds`, `SettingsManager::addLog` | Threshold behavior review | Current |
| FR-09 Display and touch bring-up | `04-bsp-and-integration/*`, `03-architecture/deployment-view.md` | Yocto overlay and module recipes | Bring-up checklist and target hardware validation | Current |
| TR-01 README as landing page | `02-solution/selected-solution.md` | `README.md` | Documentation review | In progress |
| TR-02 Separate current/target/future in docs | `02-solution/migration-strategy.md`, `03-architecture/as-is-architecture.md`, `03-architecture/target-architecture.md` | Public docs set | Documentation review | In progress |
| TR-04 Product-side app packaging completion | `04-bsp-and-integration/packaging-path.md` | `yocto/meta-system-monitor/` scaffold | Packaging review | Target |

## Current Traceability Gaps

- verification is still mostly manual and document-driven
- no committed automated test suite exists yet for parser, persistence, or startup behavior
- evidence for target-hardware bring-up is checklist-based rather than automated
