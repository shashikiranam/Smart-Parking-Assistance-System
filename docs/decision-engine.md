# Decision Engine

## Distance-based severity classification

Each of the four corner sensors is independently classified using the
thresholds below (see `config.h` / `decision_engine.c`):

| Distance range | Severity | Buzzer pattern |
|---|---|---|
| > 100 cm | SAFE | Off (silent) |
| 60 – 100 cm | CAUTION | Slow beep (~700 ms period) |
| 30 – 60 cm | WARNING | Medium beep (~350 ms period) |
| 10 – 30 cm | CRITICAL | Fast beep (~150 ms period) |
| < 10 cm | EMERGENCY | Continuous tone |

The **overall severity** reported to the LCD/LED/buzzer is the *worst*
severity across all four corners — i.e. a single close obstacle at any
corner is enough to trigger the corresponding warning, even if the other
three corners are clear.

## Steering guidance

The decision engine compares the closest reading on the left side
(`min(front_left, rear_left)`) against the closest reading on the right
side (`min(front_right, rear_right)`):

- If the **left** side is meaningfully closer to an obstacle than the
  right (by more than an 8 cm deadband), the system suggests **Turn
  Right** — steering the vehicle away from the closer obstacle.
- If the **right** side is meaningfully closer, it suggests **Turn Left**.
- The 8 cm deadband prevents the display from flickering between
  suggestions when the vehicle is essentially centered and sensor noise
  causes small left/right fluctuations.

## Priority order

Because incorrect steering guidance is a nuisance but an un-flagged
collision risk is a safety issue, the decision engine evaluates conditions
in strict priority order:

1. **STOP** — either a front or rear corner has reached CRITICAL or
   EMERGENCY range. This always overrides steering guidance.
2. **Turn Right / Turn Left** — evaluated only when no corner is critical.
3. **Centered** — left/right sides are balanced, but overall severity is
   still CAUTION or WARNING (something is close, just not asymmetrically).
4. **Proceed Slowly** — all four corners report SAFE range.

## Example scenarios

| FL | FR | RL | RR | Severity | Action |
|---|---|---|---|---|---|
| 150 | 150 | 150 | 150 | SAFE | Proceed Slowly |
| 45 | 70 | 60 | 90 | WARNING | Turn Right *(left side closer)* |
| 90 | 22 | 85 | 30 | CRITICAL | STOP *(front-right critical)* |
| 55 | 58 | 52 | 56 | WARNING | Centered *(balanced, nothing critical)* |
