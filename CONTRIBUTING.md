# Contributing

## Scope And Evidence

Keep the application and BSP layer separate. Preserve behavior
unless a change has a focused acceptance test. Do not claim MISRA, CERT, or
production compliance without rule-set, tool, deviation, and review evidence.

## Source Conventions

| Area | Primary convention | Review guidance | Comments |
| --- | --- | --- | --- |
| Qt/C++ | Qt style, C++17, RAII, const correctness | Tailored MISRA C++:2023 and CERT C++:2016 review | Doxygen for public contracts only |
| QML | Qt QML conventions and `qmllint` | Input, state, and 320x240 review | Only non-obvious input/timing constraints |
| Kernel C | Linux kernel coding style | Selected MISRA C:2023 and CERT C review | kernel-doc for state and hardware contracts |
| Device tree | Linux DTS conventions | `dtc` validation | Concise hardware constraints only |
| Yocto | BitBake layer conventions | License and dependency review | README per layer only |

Do not write Doxygen for trivial getters, signals, setters, or private helpers.
Do not use Doxygen in kernel drivers. Do not add banner comments that repeat
the code below them.

## Licensing And Metadata

Application, QML, driver, device-tree, and diagnostic utility source use
`GPL-2.0-only` SPDX metadata. Yocto BSP metadata uses MIT where declared.
Version `1.0.0` is owned by CMake and module metadata; do not add version
banners to every source file.

## Validation

Run the smallest relevant checks before submitting a change:

```sh
git diff --check
/home/tungnhs/Qt/6.10.0/gcc_64/bin/qmllint -I app/qml <changed-qml-files>
cmake -S . -B /tmp/system-monitor-check \
  -DCMAKE_BUILD_TYPE=Debug \
  -DCMAKE_PREFIX_PATH=/home/tungnhs/Qt/6.10.0/gcc_64
cmake --build /tmp/system-monitor-check -j4
```

For BSP changes, use `dtc`, `checkpatch.pl`, and the recipe build sequence in
[Yocto Build And Deploy](docs/04-yocto-build-deploy.md). Report local, Yocto,
and board results separately.
