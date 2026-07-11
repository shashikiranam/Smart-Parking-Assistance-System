# Future Improvements

Ideas for extending this project beyond its current portfolio scope:

## Sensing
- Replace HC-SR04 with waterproof/automotive-grade ultrasonic modules
  (e.g. JSN-SR04T) for real vehicle mounting.
- Add temperature compensation for the speed-of-sound constant using a
  cheap I2C temperature sensor (accuracy degrades a few % over a wide
  temperature range with a fixed constant).
- Fuse readings from adjacent corners to estimate obstacle *angle*, not
  just distance.

## Control & UX
- Replace the 16x2 character LCD with a small TFT for a graphical top-down
  parking view.
- Add a rear-view camera feed (would require a higher-tier MCU with
  DCMI/USB support, e.g. STM32H7).
- Add persistent settings (threshold tuning, units) stored in Flash via
  the HAL FLASH driver.

## Reliability
- Add IWDG (independent watchdog) servicing tied to FSM liveness, so a
  stuck state transitions to a hardware reset rather than just the
  software `ERROR` state.
- Add CRC-protected configuration storage.
- Migrate the debug UART to DMA + ring buffer for non-blocking transmit.

## Testing
- Add a hardware-in-the-loop (HIL) test harness that replays recorded
  echo-timing sequences into the EXTI handlers for automated regression
  testing without physical sensors.
- Port the `filter.c` / `decision_engine.c` modules (already
  dependency-free of HAL) into a host-side unit test suite using a
  standard C testing framework.

## Connectivity
- Add a low-power BLE module for a companion mobile app showing live
  distances (this is where the "premium vehicle" analogy ends — a full
  companion app is out of scope for this portfolio project but a natural
  next step).
