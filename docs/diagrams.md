# Diagrams

These diagrams describe the manual Raspberry Pi bring-up workflow. Flowcharts
show context and runtime layers. The class and sequence diagrams are UML-style
views derived from the current source.

## 1. System Context

```mermaid
flowchart LR
    Engineer["Engineer"] --> Host["Linux host"]
    Host --> Yocto["Yocto and SDK"]
    Host --> Deploy["SCP deployment"]
    Deploy --> Pi["Raspberry Pi 3 64-bit"]
    Pi --> Display["ILI9341 panel"]
    Pi --> Touch["XPT2046 controller"]
    Pi --> App["Qt System Monitor"]
    App --> Operator["Operator"]
```

## 2. Application and Linux Runtime Layers

```mermaid
flowchart TB
    QML["QML pages and components"] --> Facade["SystemController: systemInfo"]
    Facade --> Metrics["SystemMetricsService"]
    Facade --> Settings["SettingsManager"]
    Facade --> Policy["WarningThresholdPolicy"]
    Metrics --> Monitors["CPU, GPU, memory, storage, network monitors"]
    Monitors --> Sources["Linux sources: /proc, /sys, commands"]
    Settings --> Store["QSettings and system commands"]

    QML --> LinuxFB["Qt linuxfb: /dev/fb1"]
    LinuxFB --> DisplayDriver["ili9341_display"]
    TouchDriver["ili9341_touch"] --> Input["Linux input event"]
    Input --> Evdev["Qt evdevtouch"]
    Evdev --> QML
```

## 3. Application Class View

```mermaid
classDiagram
    class SystemController {
        +saveSettings()
        +reboot()
        +shutdown()
        +updateAllMetrics()
    }
    class SystemMetricsService {
        +collectSnapshot()
    }
    class SettingsManager {
        +save()
        +load()
        +reboot()
        +shutdown()
    }
    class WarningThresholdPolicy {
        +cpuLevel()
        +ramLevel()
    }
    class SystemMetricsSnapshot
    class CpuMonitor
    class GpuMonitor
    class MemoryMonitor
    class StorageMonitor
    class NetworkMonitor
    class QTimer

    SystemController "1" *-- "1" SystemMetricsService
    SystemController "1" *-- "1" SettingsManager
    SystemController "1" *-- "1" QTimer
    SystemController ..> WarningThresholdPolicy
    SystemMetricsService --> SystemMetricsSnapshot
    SystemMetricsService "1" *-- "1" CpuMonitor
    SystemMetricsService "1" *-- "1" GpuMonitor
    SystemMetricsService "1" *-- "1" MemoryMonitor
    SystemMetricsService "1" *-- "1" StorageMonitor
    SystemMetricsService "1" *-- "1" NetworkMonitor
```

## 4. Metric Refresh Sequence

```mermaid
sequenceDiagram
    participant Timer as QTimer
    participant Controller as SystemController
    participant Metrics as SystemMetricsService
    participant Monitors as Monitor objects
    participant Linux as /proc, /sys, commands
    participant Settings as SettingsManager
    participant Policy as WarningThresholdPolicy
    participant QML as QML bindings

    Timer->>Controller: timeout
    Controller->>Metrics: collectSnapshot()
    Metrics->>Monitors: collect CPU, GPU, memory, storage, network
    Monitors->>Linux: read runtime sources
    Linux-->>Monitors: raw values or fallback
    Monitors-->>Metrics: monitor values and histories
    Metrics-->>Controller: SystemMetricsSnapshot
    Controller->>Settings: uptime() and systemTime()
    Controller->>Policy: classify CPU and RAM
    Controller-->>QML: changed property notifications
```

## 5. Settings and Guarded System Action

```mermaid
sequenceDiagram
    participant User
    participant QML as Settings QML
    participant Dialog as ConfirmDialog
    participant Controller as SystemController
    participant Settings as SettingsManager
    participant Store as QSettings
    participant OS as Linux command

    User->>QML: edit setting
    QML->>Controller: property setter
    Controller->>Settings: update setting and append runtime log
    User->>QML: save changes
    QML->>Controller: saveSettings()
    Controller->>Settings: save()
    Settings->>Store: sync()

    User->>QML: request reboot or shutdown
    QML->>Dialog: show()
    Note over Dialog: confirm arms after 350 ms
    User->>Dialog: confirm release
    Dialog->>Controller: accepted()
    Controller->>Settings: reboot() or shutdown()
    Settings->>OS: start detached command
```

## 6. BSP Bind and Manual Deployment

```mermaid
sequenceDiagram
    participant Engineer
    participant Host as Yocto host
    participant Boot as FAT boot partition
    participant Firmware as Raspberry Pi firmware
    participant Loader as Kernel module loader
    participant SPI as Linux SPI core
    participant Display as ili9341_display
    participant Touch as ili9341_touch
    participant Devices as Linux devices

    Engineer->>Host: build overlay and modules
    Host->>Boot: copy ili9341-xpt2046-overlay.dtbo
    Engineer->>Boot: enable dtoverlay in config.txt
    Firmware->>SPI: create CE0 and CE1 DT devices
    Engineer->>Loader: install modules, depmod, modprobe display
    Loader->>SPI: register ili9341_display driver
    SPI->>Display: match ilitek,ili9341 and probe
    Display->>Devices: register /dev/fb1
    Engineer->>Loader: modprobe touch
    Loader->>SPI: register ili9341_touch driver
    SPI->>Touch: match xptek,xpt2046 and probe
    Touch->>Devices: register /dev/input/eventX
```

## 7. Display Flush Sequence

```mermaid
sequenceDiagram
    participant Qt as Qt linuxfb
    participant FB as /dev/fb1 and fbdev
    participant Deferred as fb_deferred_io
    participant Driver as ili9341_display
    participant SPI as SPI0 CE0
    participant Panel as ILI9341 panel

    Note over Driver,Panel: Probe sets MADCTL BGR for this panel
    Qt->>FB: write RGB565 pixels
    FB->>Deferred: schedule refresh after 1/fps delay
    Deferred->>Driver: ili9341_deferred_io()
    Driver->>Driver: set full window and swap pixel byte order
    Driver->>SPI: transmit RGB565 high byte first
    SPI->>Panel: RAM write
```

## 8. Touch Input Sequence

```mermaid
sequenceDiagram
    participant Finger
    participant IRQ as GPIO17 PENIRQ
    participant Driver as ili9341_touch
    participant SPI as SPI0 CE1
    participant Controller as XPT2046
    participant Input as Linux input subsystem
    participant Qt as Qt evdevtouch
    participant QML as QML controls

    Finger->>IRQ: pen down, active low
    IRQ->>Driver: falling threaded IRQ
    Driver->>Driver: schedule delayed work
    Driver->>SPI: read five X, Y, Z samples
    SPI->>Controller: SPI transfers
    Controller-->>Driver: raw samples
    Driver->>Driver: median, pressure, calibration, jitter filter
    Driver->>Input: ABS_X, ABS_Y, pressure, BTN_TOUCH
    Input->>Qt: input event
    Qt->>QML: touch or drag event
    Note over Driver: release after two pen-up polls
```
