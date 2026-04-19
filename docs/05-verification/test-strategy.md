# Test Strategy

## Current Validation Reality
The repository currently relies on:
- manual desktop-oriented checks
- manual hardware bring-up checks
- code and document review

## Recommended Automated Coverage
- parser tests for `/proc` and `/sys` readers
- persistence tests for settings
- desktop startup smoke tests
- regression tests for fallback behavior

## What Must Stay Manual
- display bring-up
- touch behavior and calibration
- end-to-end interaction on the target hardware
