# M276 Shared Native Build Helper For Readiness Runners And Checkers Expectations (D002)

Contract ID: `objc3c-shared-native-build-helper/m276-d002-v1`

## Required outcomes

- One shared helper must own native build acquisition for readiness runners and checkers.
- The helper must support:
  - `fast`
  - `contracts`
  - `full`
  - deterministic `--force-reconfigure`
- The helper must emit stable summaries/logs under `tmp/`.
- At least two active readiness runners must consume the helper instead of raw `npm run build:objc3c-native`.
- `M276-D001` must consume this helper for the broader runner migration.

## Required implementation surface

- `scripts/ensure_objc3c_native_build.py`
- `scripts/build_objc3c_native.ps1` support for `-ForceReconfigure`
- active readiness-runner adoption in the current open-work range

## Proof obligations

- helper `fast` mode produces a successful summary
- helper `contracts` mode produces a successful summary
- helper `full --force-reconfigure` produces a successful summary and records forced reconfigure behavior
- issue-local evidence lives under `tmp/reports/m276/M276-D002/`
