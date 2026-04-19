# Product Overview

## Product Summary
System Monitor is an embedded monitoring product for Raspberry Pi-class hardware using:
- an ILI9341 320x240 SPI display
- an XPT2046 resistive touch controller
- a Qt/QML userspace application
- custom BSP-side integration through Yocto, device tree, and kernel modules

## Problem Statement
The product gives operators a dedicated local interface for reading system health without requiring a desktop session, SSH session, or external monitor.

## Brownfield Status
This repository already contains:
- a working userspace monitor application
- a working BSP integration path for display and touch
- project restructuring and documentation work in progress

This is not a blank-slate project.

## Current-State Baseline
Implemented today:
- dashboard view
- CPU detail
- memory detail
- storage detail
- network detail
- settings and system actions
- display and touch BSP integration

Known current gaps:
- no dedicated GPU detail page
- automated tests are not yet committed
- final product-side Yocto packaging for the main app is still scaffolded

## Target Design Intent
The target direction is a cleaner enterprise-style monorepo where:
- README is a concise landing page
- detailed design is stored in versioned docs
- application, BSP, and product-layer concerns are separated
- architecture decisions are recorded explicitly
