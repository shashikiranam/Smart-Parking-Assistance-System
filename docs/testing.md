# Testing Procedure

This document describes a realistic, reproducible bring-up and validation
procedure for the hardware described in [`../hardware/README.md`](../hardware/README.md).
No specific numeric results, logs, or captures are claimed here — actual
values will depend on your specific sensor units, wiring, and environment.
Run these steps yourself and record your own results.

## 1. Power-on / self-test verification

1. Flash the firmware (see `firmware/README.md`).
2. Open a serial terminal (115200 8N1) on the ST-LINK VCP port.
3. Reset the board. Confirm the UART prints the startup banner and either
   a `SELF_TEST PASS` line or a fault message.
4. Confirm the LCD shows the project name followed by "Self-Test OK" (or
   the fault screen, if a sensor/LCD issue is present — check wiring).

## 2. Individual sensor verification

For each of the four HC-SR04 sensors:

1. Press the parking-mode button to enter `SENSOR_SCAN`.
2. Place a flat object (e.g. a book) directly in front of that sensor at a
   known distance (measure with a tape measure).
3. Compare the UART telemetry line's reported distance for that corner
   against your tape-measure reading. Expect agreement within a few
   centimeters at typical indoor test distances — HC-SR04 accuracy is
   inherently limited by beam angle, ambient temperature, and target
   surface (soft/angled surfaces reflect poorly).
4. Move the object further away, then remove it entirely, and confirm the
   reading trends up and eventually reports "no echo" (displayed as `---`
   over UART) beyond the sensor's rated range (~4 m).

## 3. Warning threshold verification

1. Slowly move a test object toward one sensor while watching the buzzer
   and LEDs.
2. Confirm the buzzer transitions: silent → slow beep → medium beep → fast
   beep → continuous tone as the object crosses the 100/60/30/10 cm
   thresholds defined in `config.h`.
3. Confirm the red/green LEDs track severity as described in
   `docs/decision-engine.md`.

## 4. Steering guidance verification

1. Place an object closer to the left-side sensors than the right (e.g.
   30 cm on the left, open air on the right).
2. Confirm the LCD "Action" line shows "Turn Right".
3. Repeat with the object on the right side; confirm "Turn Left".
4. Remove all objects; confirm "Proceed Slowly".

## 5. Servo sweep verification

1. Observe the SG90 horn while in `SENSOR_SCAN`/`FILTER_DATA`.
2. Confirm it visibly steps through approximately 0°, 45°, 90°, 135°,
   180°, and back to 0°, pausing briefly at each position.

## 6. Fault recovery verification

1. Temporarily disconnect one sensor's echo wire.
2. Reset the board; confirm `SELF_TEST` reports a fault for that sensor
   and the system enters `ERROR` (red LED fast-blinking, fault code on
   LCD).
3. Reconnect the sensor and press the parking-mode button; confirm the
   system re-runs self-test and recovers to normal operation.

## Screenshots / captures

Any images referencing this testing procedure in `images/results/` are
**concept illustrations only** — mockups showing what the LCD/UART output
is intended to look like — and are explicitly labeled as such. They are
not claims of actual bench measurements or hardware runs.
