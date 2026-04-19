# Requirements

## Functional Requirements

| ID | Requirement | Status |
|----|-------------|--------|
| FR-01 | Show dashboard-level system summary on the embedded display | Current |
| FR-02 | Show CPU detail metrics including utilization, temperature, and load context | Current |
| FR-03 | Show memory detail metrics including RAM and swap usage | Current |
| FR-04 | Show storage usage and I/O-related information | Current |
| FR-05 | Show network interface and traffic information | Current |
| FR-06 | Support settings and system actions such as reboot and shutdown | Current |
| FR-07 | Persist user settings across restarts | Current |
| FR-08 | Support display and touch bring-up on Raspberry Pi target hardware | Current |

## Delivery And Integration Requirements

| ID | Requirement | Status |
|----|-------------|--------|
| DIR-01 | Package the main product cleanly through a product-side Yocto layer | Target |
| DIR-02 | Provide reviewable architecture and verification documentation in versioned text form | Target |

## Non-Functional Requirements

| ID | Requirement |
|----|-------------|
| NFR-01 | Keep the UI usable on 320x240 resolution |
| NFR-02 | Keep the app responsive on Raspberry Pi-class hardware |
| NFR-03 | Degrade gracefully when some metrics are unavailable |
| NFR-04 | Keep diagrams and docs versionable as text |
| NFR-05 | Keep repo zones explicit for app, BSP, product layer, and docs |
| NFR-06 | Support both desktop-oriented app validation and hardware bring-up validation |
| NFR-07 | Keep architecture and verification artifacts reviewable and consistent with the current brownfield state |

## Out Of Scope For The Current Brownfield Cleanup
- redesigning the product into multiple repositories
- replacing Qt/QML with another UI stack
- replacing Yocto with a different build system
- claiming complete automated coverage before tests actually exist

## Current Gaps Against Target Requirements
- product-side Yocto packaging is scaffolded but not final
- architecture decision records did not previously exist
- diagram set was incomplete and inconsistent
- README was acting as a full design dump instead of a landing page
