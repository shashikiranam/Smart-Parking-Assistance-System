# Hardware

## Bill of Materials (BOM)

| Qty | Component | Notes |
|---|---|---|
| 1 | STM32 NUCLEO-G071RB | STM32G071RBT6, Arm Cortex-M0+, on-board ST-LINK |
| 4 | HC-SR04 ultrasonic distance sensor | Front-Left, Front-Right, Rear-Left, Rear-Right |
| 1 | 16x2 character LCD with PCF8574 I2C backpack | Address 0x27 (default) |
| 1 | SG90 micro servo motor | Sensor-sweep mount |
| 1 | Piezo buzzer (active or passive, driven as digital on/off) | |
| 1 | Red LED + current-limiting resistor (~330Ω) | |
| 1 | Green LED + current-limiting resistor (~330Ω) | |
| 1 | Momentary push button | Parking mode enable/disable |
| — | Breadboard + jumper wires | Prototyping |
| — | USB Micro-B / USB-C cable (board-dependent) | Power + programming |
| — | External 5V supply (optional) | Recommended once all 4 sensors + servo + LCD are attached simultaneously, as USB power alone may be marginal |

## Wiring overview

See [`../docs/pin-configuration.md`](../docs/pin-configuration.md) for the
complete pin-by-pin table. In summary:

- Each HC-SR04's **VCC** connects to 5V, **GND** to common ground.
- Each HC-SR04's **TRIG** connects to its assigned MCU GPIO output pin.
- Each HC-SR04's **ECHO** connects to its assigned MCU GPIO (EXTI) input.
  Because HC-SR04 ECHO output swings to 5V and the STM32G071 is a 3.3V
  part, a resistor divider (e.g. 1kΩ series + 2kΩ to GND) is recommended
  on each ECHO line for signal integrity/protection margin.
- The LCD backpack's **SDA/SCL** connect to the MCU's I2C1 pins, with the
  backpack's own pull-ups typically sufficient (verify with a multimeter
  if the bus doesn't ACK).
- The SG90 servo's signal wire connects to the PWM pin; servo **must** be
  powered from the 5V rail, not the MCU's 3.3V output.
- LEDs connect through current-limiting resistors to their assigned GPIO
  pins, with the other leg to ground (or per whichever polarity your board
  variant uses).
- The push button connects between its GPIO pin and ground (internal
  pull-up is enabled in firmware, so no external resistor is required).

## Power budget notes

The SG90 servo can draw several hundred mA momentarily when moving under
load; combined with four ultrasonic sensors and an LCD backlight, total
current draw can exceed what a laptop USB port comfortably supplies. For
sustained bench testing, use a dedicated 5V/1A (or greater) supply on the
5V rail rather than relying solely on the ST-LINK USB connection.

## Diagrams

Block and circuit diagrams are referenced (not embedded as generated
binary images) at:
- `../images/hardware/block_diagram.png`
- `../images/hardware/circuit_diagram.png`

See `../images/README.md` for details on how these are intended to be
produced (e.g. in Fritzing / KiCad) and added by the repository maintainer.
