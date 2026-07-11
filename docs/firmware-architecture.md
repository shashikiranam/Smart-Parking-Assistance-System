# Firmware Architecture

## Layered design

The firmware follows a layered architecture that separates hardware access
from application logic, so individual modules can be tested, replaced, or
ported independently.

```
┌─────────────────────────────────────────────────────────┐
│                     Application Layer                    │
│   parking.c  (FSM step functions / orchestration)         │
│   decision_engine.c  (severity + steering decision)       │
└───────────────┬────────────────────────┬──────────────────┘
                │                        │
┌───────────────▼──────────┐  ┌──────────▼──────────────────┐
│      Sensor Manager        │  │        Board Support         │
│   ultrasonic.c              │  │   pin_map.h / config.h        │
│   filter.c                   │  │   system_state.c (FSM core)   │
└───────────────┬──────────┘  └──────────┬──────────────────┘
                │                        │
┌───────────────▼────────────────────────▼──────────────────┐
│                      Driver Layer                          │
│  servo.c   lcd_i2c.c   buzzer.c   led.c   uart_debug.c      │
└───────────────┬──────────────────────────────────────────┘
                │
┌───────────────▼──────────────────────────────────────────┐
│                   STM32 HAL / CMSIS                         │
│         (Drivers/, generated via STM32CubeMX)                │
└─────────────────────────────────────────────────────────┘
```

## Module responsibilities

- **Application layer** (`parking.c`, `decision_engine.c`) contains no
  direct register or HAL calls; it only calls into the driver layer through
  well-defined APIs. This is what makes the FSM straightforward to unit
  test on a desktop compiler if desired (with the driver layer mocked).
- **Sensor manager** (`ultrasonic.c`, `filter.c`) owns the timing-sensitive
  EXTI-based echo measurement and the median/EMA noise filtering, exposing
  simple "trigger / poll / read" semantics to the application layer.
- **Board support** (`pin_map.h`, `config.h`, `system_state.c`) centralizes
  every physical pin assignment and tunable constant in one place, and
  implements the generic (hardware-agnostic) finite state machine
  bookkeeping.
- **Driver layer** wraps each individual peripheral (servo PWM, LCD over
  I2C, buzzer GPIO, LEDs, debug UART) behind a small, purpose-built API.

## Data flow (one scan cycle)

1. `SENSOR_SCAN` — all four HC-SR04 sensors are triggered; the servo
   advances one step of its sweep sequence.
2. `FILTER_DATA` — once all four echoes have returned (or timed out), raw
   distances are pushed through the median + EMA filter chain.
3. `PROCESS_DATA` — reserved extension point (currently pass-through).
4. `DECISION_ENGINE` — filtered distances are classified into a severity
   level per corner; the worst corner determines overall severity, and a
   left/right comparison determines steering guidance.
5. `DISPLAY_UPDATE` — the LCD is refreshed (rate-limited to
   `LCD_UPDATE_PERIOD_MS`) with live distances and the recommended action.
6. `WARNING_CONTROL` — the buzzer pattern and LED severity indication are
   updated to match the current decision.
7. `UART_UPDATE` — a telemetry line is transmitted (rate-limited to
   `UART_UPDATE_PERIOD_MS`), then the FSM loops back to `SENSOR_SCAN` (or
   returns to `IDLE` if parking mode was disabled mid-cycle).

## Why EXTI-timed echo measurement instead of hardware input capture?

The original hardware input-capture approach (dedicating a timer channel
per echo pin) would require all four echo pins to sit on timer-capable
alternate-function pins, which conflicts with the desire to keep trigger
and echo pins logically grouped per corner on the NUCLEO-G071RB's exposed
headers. Instead, all four echo pins are configured as independent EXTI
interrupt sources (rising + falling edge), and the pulse width is computed
by timestamping both edges against a single free-running 1MHz timer
(`TIM2`). This gives the same microsecond-level accuracy as hardware input
capture while keeping the pin assignment flexible.
