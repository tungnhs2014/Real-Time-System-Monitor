# Target Architecture

## Purpose

This document describes the intended target architecture direction for the brownfield System Monitor repository.

It is not a claim that every target element is already implemented.

## Target Repository Shape

The target repository remains a single product monorepo with explicit zones:
- `app/`: userspace Qt/QML product application
- `yocto/meta-rpi-ili9341-bsp/`: active hardware-facing BSP layer
- `yocto/meta-system-monitor/`: product-side packaging and product integration layer
- `docs/`: public design and verification source of truth
- `private-docs/ai/`: internal AI workflow rules only

## Target Architectural Principles

- preserve one-repo product ownership while keeping responsibilities explicit
- keep current implementation truth separate from target direction
- let runtime modularization happen incrementally rather than through a full rewrite
- keep product packaging separate from BSP support
- keep diagrams, decisions, and verification material versioned in text form

## Target Runtime Direction

### Runtime baseline to preserve

The target direction intentionally preserves these current realities during migration:
- the app continues to be a Qt/QML shell with C++ backend services
- `SystemController` remains the operational aggregation baseline until replacement slices are designed and validated
- monitor classes continue to own Linux metric collection

### Runtime direction to evolve toward

Target direction:
- reduce the breadth of `SystemController` over time
- move toward clearer page-oriented ownership boundaries
- keep low-level metric collection in dedicated C++ modules
- keep QML focused on presentation, interaction, and lightweight page flow

This target does not require an immediate rewrite into a new framework or a wholly different runtime pattern.

## Target Product-Layer Packaging Direction

Target direction:
- keep `meta-rpi-ili9341-bsp/` focused on overlay, kernel modules, kernel patch/config support, and BSP bring-up helpers
- complete `meta-system-monitor/` as the home for the product application recipe and product-side integration
- avoid collapsing product packaging back into the BSP layer

## Target Documentation Architecture

The documentation target is:
- `01-product/` defines context, requirements, and use cases
- `02-solution/` records options, decisions, and migration strategy
- `03-architecture/` keeps separate `as-is`, `target`, and detailed architecture views
- `04-bsp-and-integration/` documents hardware-facing integration truth
- `05-verification/` ties requirements to validation evidence

## Target Review Outcomes

When the target documentation architecture is working correctly, a reviewer should be able to answer:
- what the product currently does
- what the chosen design direction is
- what has not been implemented yet
- how app, BSP, and product-layer concerns are separated
- how each important requirement is meant to be verified

## Explicit Non-Claims

This target architecture does not currently claim:
- a completed page-oriented runtime refactor
- a completed product-side Yocto packaging path
- a dedicated GPU detail workflow
- committed implementation of broader future ideas from `Project.txt`
