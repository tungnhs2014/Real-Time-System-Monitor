# Runtime Architecture Options

## Decision Context
The repository structure decision was handled separately.

This document focuses on runtime application architecture around the existing monitor app.

## Brownfield Baseline
Current runtime characteristics:
- `SystemController` is the main aggregation facade
- monitor logic is separated into model classes
- QML still owns significant page-flow behavior
- no dedicated GPU detail page exists

## Option R1: Keep the current aggregator-centric architecture

### Summary
Keep `SystemController` as the primary orchestration and QML-facing state hub, with only limited cleanup.

### Advantages
- lowest migration risk
- matches the current implementation closely
- easiest to maintain during brownfield normalization

### Drawbacks
- controller remains broad
- page-level boundaries are less explicit
- scalability for future growth is limited

## Option R2: Move toward page-oriented view models gradually

### Summary
Keep the current app working, but evolve toward clearer page-level ownership such as dashboard and detail-specific state modules over time.

### Advantages
- improves modularity incrementally
- better long-term separation of concerns
- still compatible with brownfield migration

### Drawbacks
- requires staged refactoring
- increases transitional complexity
- documentation and code can diverge if migration is partial

## Option R3: Full runtime redesign before further work

### Summary
Refactor the runtime app architecture aggressively before further documentation or packaging work.

### Advantages
- strongest long-term architectural cleanup

### Drawbacks
- highest delivery risk
- poor fit for current brownfield stabilization needs
- likely to disrupt working behavior

## Comparison

| Criterion | R1 | R2 | R3 |
|-----------|----|----|----|
| Brownfield safety | High | Medium | Low |
| Architectural cleanliness | Medium | High | High |
| Refactor cost | Low | Medium | High |
| Near-term practicality | High | High | Low |

## Recommendation
Choose **R2 as target direction**, while keeping **R1 as the current operational baseline**.

This means:
- document the existing runtime truth honestly
- avoid pretending the refactor is already done
- allow gradual movement toward page-oriented state boundaries later
