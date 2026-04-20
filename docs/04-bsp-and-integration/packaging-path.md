# Packaging Path

## Purpose

This document clarifies the current and target packaging path for the product so that BSP integration and product packaging are not conflated.

## Current Packaging Reality

Current state:
- the application is currently built from `app/` with CMake
- the active Yocto layer with implemented content is `yocto/meta-rpi-ili9341-bsp/`
- `yocto/meta-system-monitor/` is present as scaffold only

This means:
- the repository already contains a valid app source path
- the repository already contains a valid BSP integration path
- the final product-side application recipe path is not yet completed

## Current BSP Packaging Scope

`yocto/meta-rpi-ili9341-bsp/` currently contains:
- display and touch kernel module recipes
- device tree overlay recipe
- kernel patch/config support
- bring-up helper recipes

These assets support hardware enablement and validation, not final ownership of the main product application packaging path.

## Intended Product Packaging Scope

`yocto/meta-system-monitor/` is intended to own:
- the main application recipe for the Qt/QML monitor
- product-side image/package composition
- product-level integration concerns that should stay separate from BSP responsibilities

## Why This Split Matters

- BSP support and product application packaging change at different rates
- reviewers need to understand whether a file exists to enable hardware or to ship the product app
- naming the whole Yocto area as `drivers` would hide important integration boundaries

## Target Packaging Path

Target direction:
1. keep the hardware enablement path in `meta-rpi-ili9341-bsp/`
2. add the final app recipe and related product integration content in `meta-system-monitor/`
3. document how the product layer consumes the app source and depends on the BSP layer

## Current Review Rule

Until a real app recipe is committed in `meta-system-monitor/`, documentation must continue to describe the product packaging path as incomplete rather than implied complete.
