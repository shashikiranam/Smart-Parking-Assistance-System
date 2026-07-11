# Contributing

Thanks for your interest in improving the Smart Parking Assistance System!

## Getting started

1. Fork the repository and clone your fork.
2. Follow `firmware/README.md` to set up STM32CubeIDE and generate the
   HAL/CMSIS driver tree.
3. Create a feature branch: `git checkout -b feature/your-feature-name`.

## Code style

- Firmware is written in C99, following the existing formatting in
  `firmware/Core/Src/` (Allman-ish brace style, `snake_case` for functions
  and variables, `SCREAMING_SNAKE_CASE` for macros/constants).
- Keep hardware pin assignments centralized in `pin_map.h` — do not
  hardcode GPIO ports/pins in application or driver `.c` files.
- Keep tunable thresholds/timings centralized in `config.h`.
- Every new public function needs a Doxygen-style header comment (see
  existing `.h` files for the expected format).
- Avoid introducing blocking delays (`HAL_Delay`) inside FSM step
  functions outside of `SELF_TEST`; the FSM is designed to be
  non-blocking so the scheduler tick stays consistent.

## Submitting changes

1. Ensure your change builds cleanly in STM32CubeIDE with no new warnings.
2. Update relevant documentation in `docs/` if you change behavior,
   thresholds, pin assignments, or the state machine.
3. Update `CHANGELOG.md` under an `[Unreleased]` heading.
4. Open a pull request describing the motivation and testing performed
   (see `docs/testing.md` for the expected manual test procedure — please
   note which steps you actually ran).

## Reporting issues

Please include:
- Firmware version / commit hash
- Board revision (NUCLEO-G071RB)
- Steps to reproduce
- Relevant UART debug output, if available

## Code of Conduct

This project follows the [Code of Conduct](CODE_OF_CONDUCT.md). By
participating, you are expected to uphold it.
