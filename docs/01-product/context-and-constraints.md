# Context And Constraints

## Context Seeding

### Product Context
- Embedded monitoring product
- Small always-on local display
- Full-stack scope from userspace UI down to BSP integration

### Stakeholder Context
- End user needs quick visibility into system health
- Developer needs a desktop-friendly development path
- Integrator needs a reproducible Yocto/BSP path
- Reviewer needs a clear separation between current implementation and target design

## Hardware Constraints
- 320x240 display resolution
- SPI-connected display and touch controller
- Resistive touch interaction
- Raspberry Pi GPIO and SPI topology constraints

## Software Constraints
- Linux userspace metrics come from `/proc`, `/sys`, filesystem APIs, and platform commands
- Qt/QML application must stay usable on constrained hardware
- BSP integration must remain compatible with Yocto layer conventions

## Brownfield Constraints
- Existing implementation already works in meaningful parts
- Documentation must not rewrite history and pretend missing features already exist
- Restructuring should preserve build and bring-up intent

## Key Risks
- mixing current state and target design in the same statements
- overloading README with architecture and troubleshooting details
- treating the whole Yocto area as only `drivers`
- renaming files, recipes, and module names inconsistently
