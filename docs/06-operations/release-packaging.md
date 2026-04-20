# Release Packaging

## Purpose

This document explains the current release and packaging reality for the repository.

## Current State

- the application source lives in `app/`
- the active hardware-facing Yocto layer lives in `yocto/meta-rpi-ili9341-bsp/`
- the intended product packaging layer lives in `yocto/meta-system-monitor/`
- `meta-system-monitor/` is still scaffolded and does not yet contain the final main app recipe

## Current Packaging Consequence

The repository supports:
- app development and desktop validation
- BSP enablement and target bring-up

The repository does not yet support:
- a documented, completed product-layer release packaging path for the main application

## Target Release Direction

Target release direction:
1. keep BSP delivery assets in `meta-rpi-ili9341-bsp/`
2. package the main app through `meta-system-monitor/`
3. document the resulting product-layer integration path in public docs

## Review Rule

Until the main app recipe exists in the product layer, release packaging must be described as incomplete.
