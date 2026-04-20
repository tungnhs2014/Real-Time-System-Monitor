# Verification Strategy

## Purpose

This document defines the verification posture for the brownfield repository.

It covers:
- what is currently verified manually
- what should gain stronger automated coverage later
- what reviewers should inspect before accepting documentation or architecture claims

## Current Verification Reality

The repository currently relies on:
- manual desktop-oriented checks
- manual hardware bring-up checks
- code and document review

This is acceptable as current reality, but it must remain explicit.

## Verification Layers

### 1. Documentation verification

Focus:
- README remains a landing page
- docs distinguish current implementation, target direction, and future ideas
- repo paths and names match the actual source tree

Primary references:
- `docs/00-start-here.md`
- `docs/01-product/*`
- `docs/02-solution/*`
- `docs/03-architecture/*`
- `docs/05-verification/known-limitations.md`

### 2. Application verification

Focus:
- app starts successfully
- dashboard and detail pages render current supported data
- settings save flow works for currently supported settings
- degraded values do not crash the UI

Primary references:
- `docs/05-verification/test-cases.md`
- `docs/05-verification/requirement-traceability.md`

### 3. BSP and integration verification

Focus:
- overlay recipe builds
- driver and recipe naming remain aligned
- target display and touch path remain coherent
- product packaging is not overstated beyond current scaffold reality

Primary references:
- `docs/04-bsp-and-integration/*`
- `docs/05-verification/bring-up-checklist.md`

## Recommended Automated Coverage

- parser tests for `/proc` and `/sys` readers
- persistence tests for settings
- desktop startup smoke tests
- regression tests for fallback behavior

## What Must Stay Manual

- display bring-up
- touch behavior and calibration
- end-to-end interaction on the target hardware

## Core Review Questions

- Are requirements, options, architecture, and verification artifacts separated cleanly?
- Do docs distinguish `as-is`, `transition`, and `target` views honestly?
- Do runtime diagrams and architecture prose match the current source implementation?
- Is the Yocto area described as layered BSP/product integration rather than as a generic driver folder?
- Are incomplete items, especially product packaging and broader future ideas, kept explicit as incomplete?

## Reporting Rule

When verification is incomplete, the reviewer should state:
- what was checked
- what was not checked
- what remains manual
- what is still only target direction rather than current implementation
