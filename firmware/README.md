# Firmware

This folder contains the complete STM32CubeIDE project for the Smart
Parking Assistance System, targeting the **STM32 NUCLEO-G071RB** board
(STM32G071RBT6, Arm Cortex-M0+).

## Folder layout

```
firmware/
├── Core/
│   ├── Inc/          Application + driver headers
│   ├── Src/           Application + driver sources
│   └── Startup/       Cortex-M0+ startup assembly (vector table)
├── Drivers/            STM32 HAL + CMSIS (generated, see Drivers/README.md)
├── cube_mx/             STM32CubeMX .ioc project (peripheral configuration)
├── .project             Eclipse/STM32CubeIDE project descriptor
├── .cproject             STM32CubeIDE build configuration
└── STM32G071RBTX_FLASH.ld  Linker script (128K Flash / 36K RAM)
```

## Prerequisites

- [STM32CubeIDE](https://www.st.com/en/development-tools/stm32cubeide.html) 1.14 or later
- [STM32CubeMX](https://www.st.com/en/development-tools/stm32cubemx.html) 6.10 or later (for regenerating `Drivers/`)
- ST-LINK/V2-1 drivers (bundled with the NUCLEO board)

## Build instructions

1. **Populate the HAL/CMSIS drivers.** See `Drivers/README.md` — either
   generate from `cube_mx/Smart-Parking-Assistance-System.ioc`, or use the
   STM32CubeIDE new-project wizard and drop in this repo's `Core/` folder.
2. Open STM32CubeIDE and choose **File → Open Projects from File System…**,
   selecting this `firmware/` directory.
3. Right-click the project → **Build Project** (or `Ctrl+B`).
4. The output binary is generated at `Debug/Smart-Parking-Assistance-System.elf`.

## Flashing instructions

1. Connect the NUCLEO-G071RB via its onboard ST-LINK USB port.
2. In STM32CubeIDE: **Run → Run** (or the green Run button). This builds
   (if needed), flashes over SWD, and starts execution automatically.
3. Alternatively, from the command line with `STM32_Programmer_CLI`:
   ```
   STM32_Programmer_CLI -c port=SWD -w Debug/Smart-Parking-Assistance-System.elf -v -rst
   ```

## Module map

| File | Responsibility |
|---|---|
| `main.c` / `main.h` | Peripheral bring-up, ISR wiring, main loop |
| `system_state.c/h` | Finite state machine bookkeeping |
| `parking.c/h` | Application orchestration (FSM step functions) |
| `decision_engine.c/h` | Severity classification + steering decision |
| `ultrasonic.c/h` | HC-SR04 trigger/echo driver (EXTI-timed) |
| `filter.c/h` | Median + EMA noise filtering |
| `servo.c/h` | SG90 sweep control (PWM) |
| `lcd_i2c.c/h` | 16x2 HD44780 LCD over I2C (PCF8574) |
| `buzzer.c/h` | Progressive buzzer warning patterns |
| `led.c/h` | Red/Green/Heartbeat LED status indication |
| `uart_debug.c/h` | UART telemetry reporting |
| `pin_map.h` | Central board pin assignment table |
| `config.h` | Tunable thresholds/timings |
| `utilities.c/h` | Small generic math/debounce helpers |
| `stm32g0xx_it.c/h` | Interrupt vector implementations |

See [`../docs/firmware-architecture.md`](../docs/firmware-architecture.md) for
a full architectural walkthrough and [`../docs/state-machine.md`](../docs/state-machine.md)
for the FSM diagram and transition table.
