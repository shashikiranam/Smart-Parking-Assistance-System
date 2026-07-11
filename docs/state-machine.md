# Finite State Machine

## State diagram

```
                ┌───────────────┐
        ┌──────▶│  SYSTEM_INIT  │
        │       └───────┬───────┘
        │               │
        │               ▼
        │       ┌───────────────┐    fail / timeout   ┌───────────┐
        │       │   SELF_TEST    ├─────────────────────▶│   ERROR   │
        │       └───────┬───────┘                       └─────┬─────┘
        │               │ pass                                 │
        │               ▼                                       │ button
        │       ┌───────────────┐◀──────────────────────────────┘  toggled
        │       │ SENSOR_SCAN    │◀────────────────────────────────────┐
        │       └───────┬───────┘                                       │
        │               ▼                                               │
        │       ┌───────────────┐                                       │
        │       │ FILTER_DATA    │ (loops on itself until all 4 echoes   │
        │       └───────┬───────┘  return or time out)                  │
        │               ▼                                               │
        │       ┌───────────────┐                                       │
        │       │ PROCESS_DATA   │                                       │
        │       └───────┬───────┘                                       │
        │               ▼                                               │
        │       ┌───────────────┐                                       │
        │       │DECISION_ENGINE │                                       │
        │       └───────┬───────┘                                       │
        │               ▼                                               │
        │       ┌───────────────┐                                       │
        │       │ DISPLAY_UPDATE │                                       │
        │       └───────┬───────┘                                       │
        │               ▼                                               │
        │       ┌───────────────┐                                       │
        │       │WARNING_CONTROL │                                       │
        │       └───────┬───────┘                                       │
        │               ▼                                               │
        │       ┌───────────────┐   parking mode still ON               │
        │       │  UART_UPDATE   ├───────────────────────────────────────┘
        │       └───────┬───────┘
        │               │ parking mode OFF
        │               ▼
        │       ┌───────────────┐   button pressed (mode ON)
        └───────┤     IDLE       ├──────────────────────────▶ (back to SENSOR_SCAN)
                └───────────────┘
```

## State table

| State | Entry condition | Exit condition(s) | Notes |
|---|---|---|---|
| `SYSTEM_INIT` | Power-on / reset | Always → `SELF_TEST` | One-shot; peripheral init happens in `parking_app_init()` before the FSM starts stepping |
| `SELF_TEST` | From `SYSTEM_INIT`, or recovery from `ERROR` | Pass → `SENSOR_SCAN`; fail/timeout → `ERROR` | Verifies all 4 sensors return plausible readings and the LCD ACKs over I2C |
| `SENSOR_SCAN` | From `UART_UPDATE` (loop) or `IDLE` | Always → `FILTER_DATA` | Triggers all 4 HC-SR04 sensors, advances servo sweep |
| `FILTER_DATA` | From `SENSOR_SCAN` | All 4 echoes ready → `PROCESS_DATA`; else stays in state | Bounded by `ECHO_TIMEOUT_US` per sensor |
| `PROCESS_DATA` | From `FILTER_DATA` | Always → `DECISION_ENGINE` | Reserved extension point |
| `DECISION_ENGINE` | From `PROCESS_DATA` | Always → `DISPLAY_UPDATE` | Runs `decision_engine_evaluate()` |
| `DISPLAY_UPDATE` | From `DECISION_ENGINE` | Always → `WARNING_CONTROL` | LCD refresh rate-limited to `LCD_UPDATE_PERIOD_MS` |
| `WARNING_CONTROL` | From `DISPLAY_UPDATE` | Always → `UART_UPDATE` | Buzzer pattern + LED severity update |
| `UART_UPDATE` | From `WARNING_CONTROL` | Parking mode ON → `SENSOR_SCAN`; OFF → `IDLE` | Telemetry rate-limited to `UART_UPDATE_PERIOD_MS` |
| `IDLE` | From `UART_UPDATE`, or system boot with parking mode disabled | Button toggled ON → `SENSOR_SCAN` | Green LED slow-blinks, buzzer silenced |
| `ERROR` | Any state, on fault (`system_state_raise_fault()`) | Button toggled → `SELF_TEST` (retry) | Red LED fast-blinks, fault code shown on LCD |

## Fault codes

| Code | Meaning |
|---|---|
| `FAULT_NONE` | No fault (default) |
| `FAULT_SELF_TEST_TIMEOUT` | Self-test exceeded `SELF_TEST_TIMEOUT_MS` |
| `FAULT_SENSOR_FL_UNRESPONSIVE` | Front-left sensor returned implausible reading |
| `FAULT_SENSOR_FR_UNRESPONSIVE` | Front-right sensor returned implausible reading |
| `FAULT_SENSOR_RL_UNRESPONSIVE` | Rear-left sensor returned implausible reading |
| `FAULT_SENSOR_RR_UNRESPONSIVE` | Rear-right sensor returned implausible reading |
| `FAULT_I2C_LCD_NACK` | LCD did not acknowledge on I2C bus |
| `FAULT_SERVO_OUT_OF_RANGE` | Reserved for future servo feedback validation |
| `FAULT_WATCHDOG_RESET` | Reserved: IWDG reset detected on boot |
