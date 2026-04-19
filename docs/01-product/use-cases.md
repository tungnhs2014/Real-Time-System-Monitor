# Use Cases

## Actors
- Local operator
- Developer
- BSP integrator
- Reviewer

## Primary Use Cases

### UC-01 View overall system health
Actor:
- Local operator

Goal:
- open the product and quickly read overall system status

### UC-02 Inspect CPU behavior
Actor:
- Local operator

Goal:
- inspect utilization, temperature, and runtime load behavior

### UC-03 Inspect memory behavior
Actor:
- Local operator

Goal:
- inspect RAM and swap usage

### UC-04 Inspect storage behavior
Actor:
- Local operator

Goal:
- inspect capacity and I/O activity

### UC-05 Inspect network behavior
Actor:
- Local operator

Goal:
- inspect bandwidth and interface state

### UC-06 Change settings
Actor:
- Local operator

Goal:
- adjust refresh, warning, and system-level behavior

### UC-07 Perform system action
Actor:
- Local operator

Goal:
- reboot or shut down the target device

## Supporting Use Cases
- UC-08 Run the app on a desktop environment for development
- UC-09 Bring up display and touch on Raspberry Pi hardware
- UC-10 Review the product from repo structure down to BSP details

## Degraded-Mode Use Cases
- UC-11 Continue rendering when one metric source is missing
- UC-12 Continue documentation and review work when target-state refactoring is not complete yet
