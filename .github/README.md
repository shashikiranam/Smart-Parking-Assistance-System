# .github

GitHub-specific configuration for this repository: issue templates, and a
CI workflow that performs a static sanity build of the STM32CubeIDE
project.

| File | Purpose |
|---|---|
| `workflows/build-check.yml` | Runs `arm-none-eabi-gcc` syntax/compile checks on push and pull request |
| `ISSUE_TEMPLATE/bug_report.md` | Structured template for bug reports |
| `ISSUE_TEMPLATE/feature_request.md` | Structured template for feature requests |
