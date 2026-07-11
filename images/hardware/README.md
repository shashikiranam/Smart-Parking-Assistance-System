# Hardware

This directory contains the hardware design resources for the **Smart
Parking Assistance System**.

It documents the physical architecture, hardware connections, sensor
placement, and supporting illustrations used during development.

> **Note:** Unless explicitly stated, the diagrams in this folder are
> documentation illustrations and **not** PCB manufacturing files or
> verified laboratory measurements.

------------------------------------------------------------------------

## Folder Contents

  -------------------------------------------------------------------------------
  File                           Description
  ------------------------------ ------------------------------------------------
  `hardware_block_diagram.png`   High-level hardware architecture showing all
                                 major modules and their interconnections.

  `circuit_diagram.png`          System wiring diagram illustrating electrical
                                 connections between the STM32 and peripherals.

  `sensor_layout.png`            Placement of ultrasonic sensors around the
                                 vehicle for obstacle detection.

  `pin_connection_diagram.png`   STM32 pin mapping for sensors, LCD, buzzer,
                                 LEDs, servo, and UART.

  `component_overview.png`       Overview of the primary hardware components used
                                 in the project.
  -------------------------------------------------------------------------------

------------------------------------------------------------------------

## Hardware Overview

The Smart Parking Assistance System is built around the **STM32
NUCLEO-G071RB** development board and interfaces with multiple
peripherals to provide real-time parking guidance.

### Main Components

-   STM32 NUCLEO-G071RB
-   4 × HC-SR04 Ultrasonic Sensors
-   16×2 LCD (I2C Interface)
-   SG90 Servo Motor
-   Piezo Buzzer
-   Red LED
-   Green LED
-   Push Button (Parking Mode)
-   USB / 5V Power Supply

------------------------------------------------------------------------

## Documentation Purpose

The hardware documentation is intended to:

-   Explain hardware architecture
-   Show sensor placement
-   Document electrical connections
-   Assist firmware development
-   Simplify future maintenance

------------------------------------------------------------------------

## Design Guidelines

All hardware diagrams follow these principles:

-   White background for print-friendly documentation
-   Clean engineering layout
-   Consistent symbols and labels
-   High-resolution PNG images
-   Suitable for GitHub and technical reports

------------------------------------------------------------------------

## Related Documentation

-   `/docs/hardware/`
-   `/docs/architecture/`
-   `/firmware/`

------------------------------------------------------------------------

## License

All documentation graphics in this directory are distributed under the
same license as this repository.
