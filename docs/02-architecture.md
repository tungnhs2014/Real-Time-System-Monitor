# Architecture

## Application Runtime

`main.cpp` creates `SystemController` and exposes it to QML as `systemInfo`.
It also selects `linuxfb` on `/dev/fb1`, finds the XPT2046 input device, loads
one `evdevtouch` backend, and sets `QT_QPA_FB_DISABLE_INPUT=1` to prevent a
second linuxfb-discovered backend from consuming the same events.

`SystemController` is a QML-facing facade, not a claim of pure MVC or MVVM. It
owns a refresh timer, `SystemMetricsService`, and `SettingsManager` through Qt
parent ownership on the GUI thread.

| Component | Responsibility |
| --- | --- |
| `SystemMetricsService` | Owns CPU, GPU, memory, storage, and network monitors; returns `SystemMetricsSnapshot`. |
| `SystemController` | Applies snapshots to QML properties, emits change signals, and evaluates warning policy. |
| `SettingsManager` | Loads/saves `QSettings`, stores runtime logs, reads static system identity, and invokes confirmed system actions. |
| `WarningThresholdPolicy` | Pure CPU/RAM severity classification with no Qt or Linux dependency. |
| QML | Renders properties and protects navigation/system actions from residual touch input. |

The refresh timer calls `SystemMetricsService::collectSnapshot()`. The facade
only emits a QML notification when a value changes, then obtains uptime/time
from `SettingsManager` and evaluates CPU/RAM thresholds. See the class and
metric sequence diagrams in [diagrams.md](diagrams.md).

## App-to-BSP Boundary

The app does not call driver APIs. The overlay creates SPI devices; the display
driver publishes `/dev/fb1`; the touch driver publishes an input event device.
Qt writes RGB565 pixels through `linuxfb` and receives touch events through
`evdevtouch`. This boundary permits app and driver iteration independently.
