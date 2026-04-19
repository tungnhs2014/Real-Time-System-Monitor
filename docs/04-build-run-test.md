# Build, Run, And Test

## Local App Build

From the repository root:

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build
ctest --test-dir build --output-on-failure
```

The app requires Qt6 Core, Gui, Quick, and Qml CMake packages. If Qt6 is unavailable, CMake configure will fail before app compilation.

## Current Automated Test

Current automated coverage is intentionally small:

- `app/tests/WarningThresholdPolicyTest.cpp`
- CTest target: `system-monitor-policy-tests`

If Qt6 is unavailable but a C++ compiler exists, the pure policy test can still be checked directly:

```sh
g++ -std=c++17 -I app/src/domain app/tests/WarningThresholdPolicyTest.cpp -o /tmp/system-monitor-policy-tests
/tmp/system-monitor-policy-tests
```

## Local Non-Board Checks

- `git status --short`
- qrc asset review against `app/resources/qml.qrc`
- docs link/path sanity with `rg`
- overlay compilation with `dtc`
- recipe/module/autoload naming review

## Board-Only Checks

These cannot be proven on a desktop machine:

- ILI9341 framebuffer output and refresh behavior
- XPT2046 touch detection, calibration, and event delivery
- end-to-end 320x240 UI fit
- systemd startup on the target image
- full Yocto image boot and product service behavior

## Board Smoke Checklist

1. Boot the target image.
2. Confirm display and touch probe logs:

```sh
dmesg | grep -Ei 'ili9341|xpt2046|spi|fb|touch'
```

3. Confirm devices exist:

```sh
ls -l /dev/fb*
ls -l /dev/input/event*
```

4. Launch or restart the app.
5. Confirm dashboard, CPU, memory, storage, network, and settings pages render at 320x240.
6. Confirm threshold save/reload works.
7. Confirm GPU card does not navigate to the wrong page.
8. Let the app run for 30 minutes without a crash.
9. Reboot and confirm drivers and app startup remain stable.

## Known Test Gaps

- parser fixtures for `/proc` and `/sys`
- settings repository tests
- QML smoke automation
- QSignalSpy tests for facade notification behavior
- kernel module build and checkpatch in a target kernel workspace
- full BitBake and board validation
