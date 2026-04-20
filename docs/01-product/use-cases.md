# Use Cases

This file lists product and engineering use cases that are grounded in the current brownfield repository.

## Actors

- Local operator
- Developer
- BSP integrator
- Reviewer

## Primary Product Use Cases

### UC-01 View overall system health

Actor:
- Local operator

Current status:
- Current

Trigger:
- Launch the application on desktop or on the target device

Main success flow:
1. The app starts on the dashboard.
2. The dashboard renders hostname, CPU summary, GPU summary, memory summary, storage summary, and network speed summary.
3. The operator uses the dashboard as the quick-glance system status view.

Notes:
- GPU is a dashboard summary only in the current implementation.

### UC-02 Inspect CPU behavior

Actor:
- Local operator

Current status:
- Current

Trigger:
- Tap the CPU dashboard card or navigate from bottom navigation to CPU detail

Main success flow:
1. The CPU detail page opens.
2. The page renders temperature history.
3. The page renders per-core usage bars.

Notes:
- Current CPU detail focuses on per-core usage and temperature history.
- Some CPU summary values such as clock and overall usage are shown on the dashboard rather than in a richer CPU detail workflow.

### UC-03 Inspect memory behavior

Actor:
- Local operator

Current status:
- Current

Trigger:
- Tap the memory dashboard card or navigate from bottom navigation to memory detail

Main success flow:
1. The memory detail page opens.
2. The page renders RAM usage percentage.
3. The page renders used, free, total, and cache values.

### UC-04 Inspect storage behavior

Actor:
- Local operator

Current status:
- Current

Trigger:
- Tap the storage dashboard card or navigate from bottom navigation to storage detail

Main success flow:
1. The storage detail page opens.
2. The page renders root partition capacity and utilization.
3. The page renders swap statistics.
4. The page renders I/O history plus current read and write rates.

### UC-05 Inspect network behavior

Actor:
- Local operator

Current status:
- Current

Trigger:
- Navigate from bottom navigation to network detail

Main success flow:
1. The network detail page opens.
2. The page renders active interface, IP address, and MAC address.
3. The page renders upload and download history.
4. The page renders packet rate and active connection count.

### UC-06 Review system information and logs

Actor:
- Local operator

Current status:
- Current

Trigger:
- Open the settings page

Main success flow:
1. The settings page opens on the system tab.
2. The page renders hostname, OS version, kernel version, uptime, and system time.
3. The logs tab renders recent system log entries.

### UC-07 Change supported warning thresholds and save settings

Actor:
- Local operator

Current status:
- Current

Trigger:
- Open the warning tab in settings, adjust values, and save

Main success flow:
1. The user changes CPU warning, CPU critical, or RAM warning thresholds.
2. The updated values flow through `SystemController` into `SettingsManager`.
3. The user saves the changes.
4. The app persists the supported values through `QSettings`.

Notes:
- Threshold editing is the clearest completed settings-edit flow in the current UI.
- The update interval control is not yet a completed selection flow on the current page.

### UC-08 Perform a system reboot or shutdown

Actor:
- Local operator

Current status:
- Current

Trigger:
- Press reboot or shutdown on the settings page and confirm

Main success flow:
1. The app shows a confirmation dialog.
2. The user confirms the action.
3. The app logs the action request.
4. The backend launches the system reboot or shutdown command.

## Supporting Engineering Use Cases

### UC-09 Run the app on a desktop environment for validation

Actor:
- Developer

Current status:
- Current

Goal:
- Configure, build, and run the Qt/QML app outside the target hardware for iteration and smoke checking.

### UC-10 Bring up display and touch on Raspberry Pi hardware

Actor:
- BSP integrator

Current status:
- Current

Goal:
- Build and deploy the overlay and kernel modules so the target panel and touch path are usable by the app.

### UC-11 Review the repository and architecture honestly as a brownfield product

Actor:
- Reviewer

Current status:
- Current target

Goal:
- Distinguish current implementation, target design direction, and incomplete migration work without inference.

## Degraded-Mode Use Cases

### UC-12 Continue rendering when a metric source is unavailable

Actor:
- Local operator

Current status:
- Current baseline

Goal:
- Keep the app running even if a temperature sensor, frequency source, GPU command, or network detail source is unavailable.

### UC-13 Continue delivery review while product packaging remains incomplete

Actor:
- Reviewer

Current status:
- Current target

Goal:
- Review the app and BSP design honestly even though `meta-system-monitor/` is not yet a completed packaging path.

## Not Current Use Cases

These are not current use cases and should not be documented elsewhere as if they already exist:

- dedicated GPU detail inspection
- top-process inspection for CPU or RAM
- service monitoring views
- remote API interaction
- backup and restore workflows
- network firmware update flows
