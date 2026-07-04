# Application Scope

The application displays CPU, GPU, memory, storage, and network metrics on a
320x240 touch screen. It provides detail pages, configurable CPU/RAM warning
thresholds, a runtime log list, persistent settings, and guarded reboot or
shutdown actions.

Metric refresh is periodic and best-effort. It is suitable for near-real-time
monitoring, not a hard real-time guarantee. Missing Linux data sources retain
safe fallback values rather than stopping the UI.

The target runtime is Qt `linuxfb` on `/dev/fb1` with one explicit
`evdevtouch` input backend. The application is cross-built with the Yocto SDK
and copied to the board manually.

Out of scope:

- a packaged image that auto-starts the application;
- remote fleet management;
- formal MISRA/CERT compliance certification;
- validation claims for Raspberry Pi boards other than the Pi 3 64-bit target.
