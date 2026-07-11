# Simulations

This folder is a placeholder for host-side / simulated verification of the
sensor-independent logic layers (`filter.c`, `decision_engine.c`,
`system_state.c`), which have no direct STM32 HAL dependency and can be
compiled and exercised on a desktop machine.

## Suggested approach

1. Create a `sim/` subfolder with a small `CMakeLists.txt` (or plain
   Makefile) that compiles `filter.c`, `decision_engine.c`, and
   `system_state.c` from `../firmware/Core/Src/` together with a
   hand-written `sim_main.c` driver.
2. In `sim_main.c`, feed synthetic `decision_input_t` distance sequences
   (e.g. a vehicle approaching a wall at a constant closing speed) into
   `decision_engine_evaluate()` and print the resulting severity/action
   trace to stdout.
3. This is useful for regression-testing changes to the threshold
   constants in `config.h` or the steering deadband logic without needing
   physical hardware.

No simulation binaries or captured output are included in this repository
— this folder documents the intended workflow for a contributor to set one
up.
