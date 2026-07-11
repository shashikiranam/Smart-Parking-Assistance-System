# Pin Configuration

Board: **STM32 NUCLEO-G071RB** (STM32G071RBT6, LQFP64)

All assignments are defined once in [`firmware/Core/Inc/pin_map.h`](../firmware/Core/Inc/pin_map.h);
this document mirrors that file for quick hardware-side reference.

## Ultrasonic sensors (HC-SR04 x4)

| Sensor | Trigger pin | Echo pin | Echo EXTI line |
|---|---|---|---|
| Front Left | PA0 | PA1 | EXTI1 |
| Front Right | PA4 | PA8 | EXTI8 |
| Rear Left | PB0 | PB11 | EXTI11 |
| Rear Right | PB10 | PB12 | EXTI12 |

> Echo pins were deliberately assigned to four *different* pin numbers
> (1, 8, 11, 12) since STM32 EXTI lines 0–15 are shared across GPIO ports —
> only one port can source a given EXTI line number at a time.

## Actuators & indicators

| Function | Pin | Peripheral |
|---|---|---|
| Servo (SG90) PWM | PA6 | TIM3_CH1 |
| Buzzer | PA7 | GPIO output (software-timed pattern) |
| LED — Green (safe) | PB3 | GPIO output |
| LED — Red (danger) | PB4 | GPIO output |
| LED — Heartbeat / fault | PA5 | GPIO output |
| Push button — Parking mode | PC13 | EXTI13 (on-board B1) |

## Communication

| Interface | Pins | Peripheral | Notes |
|---|---|---|---|
| LCD (I2C) | PB8 (SCL), PB9 (SDA) | I2C1 | 100 kHz, PCF8574 backpack @ address 0x27 |
| Debug UART | PA2 (TX), PA3 (RX) | USART2 | 115200 8N1, routed to ST-LINK Virtual COM Port |

## Internal timing

| Resource | Purpose |
|---|---|
| TIM2 (32-bit, 1 MHz) | Free-running microsecond counter for echo pulse-width timing |
| TIM3 (PWM, 50 Hz) | Servo position control (500–2500 µs pulse width) |
| SysTick | 1 ms system tick (`HAL_GetTick()`), drives the FSM scheduler |

## Power

| Rail | Source | Notes |
|---|---|---|
| 5V | NUCLEO 5V header (from USB or external supply) | Powers HC-SR04 sensors, buzzer, LCD backlight |
| 3.3V | NUCLEO 3.3V header | Powers LCD logic (via backpack regulator, if fitted) and LEDs |
| Logic level | 3.3V (MCU) | HC-SR04 ECHO output is 5V tolerant on most breakout boards, but a resistor divider (e.g. 1kΩ/2kΩ) is recommended on ECHO lines for safety margin |
