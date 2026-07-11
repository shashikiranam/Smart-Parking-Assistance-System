# Changelog

All notable changes to this project are documented in this file.
The format follows [Keep a Changelog](https://keepachangelog.com/en/1.1.0/),
and this project uses [Semantic Versioning](https://semver.org/).

## [1.0.0] - 2026-07-11

### Added
- Initial public release of the Smart Parking Assistance System firmware.
- Finite state machine covering init, self-test, sensor scan, filtering,
  decision engine, display update, warning control, UART telemetry, idle,
  and error/fault recovery states.
- Four-corner HC-SR04 ultrasonic sensing via EXTI-timed echo measurement.
- Median + exponential moving average noise filtering.
- Severity classification (Safe / Caution / Warning / Critical / Emergency)
  with progressive buzzer patterns and LED indication.
- Steering guidance (Turn Left / Turn Right / Centered / Proceed Slowly /
  STOP) via the decision engine.
- SG90 servo sweep scanning (0°–180° in five steps).
- 16x2 I2C LCD live status display.
- UART debug telemetry stream.
- Complete STM32CubeIDE project structure for the NUCLEO-G071RB.
- Full documentation set (architecture, state machine, pin configuration,
  decision engine, testing procedure, future improvements).

[1.0.0]: https://github.com/your-username/Smart-Parking-Assistance-System/releases/tag/v1.0.0
