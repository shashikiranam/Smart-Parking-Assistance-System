# Architecture Diagrams

This directory contains the high-level architectural illustrations for
the **Smart Parking Assistance System**.

These diagrams are intended to help developers understand the overall
system design, firmware organization, and data flow.

> **Note:** These are documentation diagrams and do not represent actual
> hardware captures.

------------------------------------------------------------------------

## Contents

  ------------------------------------------------------------------------------
  File                          Description
  ----------------------------- ------------------------------------------------
  `system_architecture.png`     Overall embedded system architecture showing the
                                STM32, sensors, peripherals, and user
                                interfaces.

  `software_architecture.png`   Layered software architecture including
                                Application, Drivers, HAL, and Hardware layers.

  `firmware_architecture.png`   Internal firmware module organization and
                                interactions.

  `data_flow.png`               End-to-end data flow from ultrasonic sensing to
                                parking guidance output.
  ------------------------------------------------------------------------------

------------------------------------------------------------------------

## Purpose

The architecture diagrams are designed to:

-   Illustrate hardware and software interactions.
-   Explain firmware modularization.
-   Show data movement between system components.
-   Improve maintainability and onboarding for contributors.

------------------------------------------------------------------------

## Diagram Standards

All architecture diagrams follow these principles:

-   Professional engineering documentation
-   Consistent naming conventions
-   Clear directional data flow
-   High-resolution graphics
-   GitHub-friendly formatting

------------------------------------------------------------------------

## Related Documentation

-   `/docs/architecture/`
-   `/docs/firmware/`
-   `/docs/hardware/`

------------------------------------------------------------------------

## License

These documentation assets are distributed under the same license as
this repository.
