# Hardware Overview

## Target Hardware
- Raspberry Pi class board
- ILI9341 SPI TFT display
- XPT2046 resistive touch controller

## Key Hardware Relationships
- display is driven over SPI
- touch controller is also accessed over SPI
- GPIO lines are used for display control and touch interrupt behavior

## Why This Matters
The product is not only a UI application. The hardware path and BSP integration are part of the architecture story and must be documented explicitly.
