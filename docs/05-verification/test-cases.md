# Test Cases

## Purpose

These test cases provide concrete validation scenarios for the current brownfield implementation.

They do not claim that all cases are automated.

## Application Smoke Cases

### TC-APP-01 Startup to dashboard

Goal:
- confirm the app starts and renders the dashboard

Method:
- manual desktop run or target run

Checks:
- application window or framebuffer UI appears
- dashboard is the initial page
- CPU, memory, storage, and network summary areas render

### TC-APP-02 CPU detail rendering

Goal:
- confirm CPU detail page shows expected current widgets

Method:
- manual

Checks:
- CPU page opens from dashboard or bottom navigation
- temperature history chart renders
- per-core usage bars render

### TC-APP-03 Memory detail rendering

Goal:
- confirm memory detail page shows RAM metrics

Method:
- manual

Checks:
- memory page opens
- RAM usage percentage is visible
- used, free, total, and cache values are visible

### TC-APP-04 Storage detail rendering

Goal:
- confirm storage page shows root, swap, and I/O data

Method:
- manual

Checks:
- storage page opens
- root partition values render
- swap values render
- I/O chart plus read/write values render

### TC-APP-05 Network detail rendering

Goal:
- confirm network page shows interface and traffic data

Method:
- manual

Checks:
- network page opens
- interface, IP, and MAC values render
- upload/download histories render
- packet rate and active connections render

### TC-APP-06 Settings persistence

Goal:
- confirm currently supported save flow persists values

Method:
- manual

Checks:
- adjust CPU or RAM threshold values
- save changes
- restart the app
- confirm saved threshold values reload

### TC-APP-07 Reboot/shutdown action confirmation

Goal:
- confirm system-action flow is gated by confirmation

Method:
- manual, safe environment only

Checks:
- reboot and shutdown buttons show confirmation dialogs
- backend call is only triggered after confirmation

## Degraded-Mode Cases

### TC-DEG-01 Missing metric source fallback

Goal:
- confirm missing metric sources degrade values rather than crash the app

Method:
- code review plus targeted manual simulation where safe

Checks:
- monitor classes return fallback values such as `0`, `N/A`, or `-1`
- UI remains usable

### TC-DEG-02 Incomplete packaging path honesty

Goal:
- confirm docs do not overstate the product packaging state

Method:
- documentation review

Checks:
- docs describe `meta-system-monitor/` as scaffold rather than complete

## BSP Bring-Up Cases

### TC-BSP-01 Overlay recipe build

Goal:
- confirm the device tree overlay recipe builds cleanly

Method:
- manual build validation

Checks:
- the overlay compiles to `.dtbo`
- output naming matches the documented path

### TC-BSP-02 Kernel module naming consistency

Goal:
- confirm recipe, source, and autoload names stay aligned

Method:
- manual review

Checks:
- `ili9341-fb` recipe aligns with `ili9341_fb.c` and module autoload name
- `xpt2046-touch` recipe aligns with `xpt2046_touch.c` and module autoload name

### TC-BSP-03 Hardware bring-up

Goal:
- confirm display and touch are usable on the Raspberry Pi target

Method:
- manual hardware validation

Checks:
- display becomes active
- touch input is detected
- the Qt app renders and remains usable at 320x240
