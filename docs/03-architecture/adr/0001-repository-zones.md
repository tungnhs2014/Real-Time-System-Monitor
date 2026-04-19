# ADR 0001: Separate Repository Zones Explicitly

## Status
Accepted

## Context
The brownfield repository originally mixed application code, Yocto/BSP assets, and extensive documentation under ambiguous top-level paths.

## Options Considered
- keep a flat repository layout
- separate repository zones inside one monorepo
- split into multiple repositories

## Decision
Keep one monorepo, but separate it into:
- `app/`
- `yocto/`
- `docs/`
- `private-docs/ai/`

## Tradeoffs
- adds migration work now
- improves long-term clarity and reviewability

## Consequences
- docs and README must be updated to the new paths
- onboarding and review become easier
