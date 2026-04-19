# Solution Option Analysis

## Decision Context
The repository is brownfield: code and BSP assets already exist, but structure and documentation needed normalization.

The design problem is not only how to organize files, but also how to document:
- requirements
- current state
- target architecture
- migration decisions

## Scope Of This Decision
This option analysis is primarily about:
- repository organization
- documentation organization
- separation between app, BSP, and product-layer concerns

It is not a full re-evaluation of the runtime monitor architecture inside `app/`.

The current runtime application structure is treated as the brownfield baseline and is documented in `docs/03-architecture/sad.md` and `docs/03-architecture/sdd.md`.

## Option A: Keep the old flat structure

### Summary
Keep top-level `src/`, `qml/`, `drivers/`, and a large README.

### Advantages
- no migration effort
- easiest short-term path

### Drawbacks
- weak separation between app and Yocto/BSP concerns
- README becomes overloaded
- `drivers/` is technically misleading when it contains a full layer
- poor fit for long-term architecture documentation

## Option B: Keep one monorepo, but separate zones clearly

### Summary
Use:
- `app/`
- `yocto/meta-rpi-ili9341-bsp/`
- `yocto/meta-system-monitor/`
- `docs/`
- `private-docs/ai/`

### Advantages
- preserves one-repo workflow
- matches Yocto layer conventions better
- cleanly separates app, BSP, and documentation concerns
- easiest to review and grow over time

### Drawbacks
- requires controlled path and naming migration
- requires docs rewrite to avoid stale references

## Option C: Split into multiple repositories

### Summary
Create separate repositories for app, BSP layer, and docs.

### Advantages
- strongest isolation of responsibilities
- can fit very large multi-team organizations

### Drawbacks
- too much overhead for the current product scale
- makes brownfield migration and synchronized review harder
- adds packaging and version coordination cost

## Comparison

| Criterion | Option A | Option B | Option C |
|-----------|----------|----------|----------|
| Short-term disruption | Low | Medium | High |
| Technical clarity | Low | High | High |
| Brownfield practicality | Medium | High | Low |
| Yocto alignment | Low | High | High |
| Reviewability | Low | High | Medium |
| Maintenance overhead | Medium | Medium | High |

## Recommendation
Choose **Option B**.

It provides the best balance between enterprise-style clarity and brownfield practicality.
