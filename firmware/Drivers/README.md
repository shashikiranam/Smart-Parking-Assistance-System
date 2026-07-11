# Drivers/ (STM32 HAL + CMSIS)

This folder is intentionally **empty** in version control.

The STM32G0xx HAL driver library and CMSIS device headers are maintained
and distributed by STMicroelectronics as part of the official
**STM32CubeG0** firmware package. To keep this repository lean, free of
vendor-redistribution concerns, and always up to date with the latest HAL
patches, those files are generated locally instead of being committed here.

## How to populate this folder

**Option A — STM32CubeMX (recommended)**

1. Open `firmware/cube_mx/Smart-Parking-Assistance-System.ioc` in
   [STM32CubeMX](https://www.st.com/en/development-tools/stm32cubemx.html).
2. Click **GENERATE CODE**.
3. Point the output directory at `firmware/`.
4. CubeMX will populate:
   - `Drivers/STM32G0xx_HAL_Driver/`
   - `Drivers/CMSIS/Device/ST/STM32G0xx/`
   - `Drivers/CMSIS/Include/`
5. Keep **"Do not generate the main()"** / **"Keep user code"** options
   enabled so the hand-written `Core/Src/main.c` in this repository is
   preserved (CubeMX merges around the `USER CODE BEGIN/END` markers).

**Option B — STM32CubeIDE new project wizard**

1. Create a new STM32 project targeting **NUCLEO-G071RB**.
2. Let STM32CubeIDE generate the default project (this creates the
   `Drivers/` folder automatically).
3. Delete the generated `Core/` folder and replace it with the `Core/`
   folder from this repository.

## Expected folder layout after generation

```
Drivers/
├── CMSIS/
│   ├── Device/ST/STM32G0xx/
│   └── Include/
└── STM32G0xx_HAL_Driver/
    ├── Inc/
    └── Src/
```

## Version used during development

- STM32CubeG0 firmware package: **v1.6.x** or later
- STM32G0xx_HAL_Driver: bundled version matching the CubeG0 package above
