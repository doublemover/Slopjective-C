# Objective-C 3 Public Command Surface

This runbook is generated from the live public workflow runner metadata.

- Current public script count: `29`
- Runner path: `scripts/objc3c_public_workflow_runner.py`
- Introspection command: `python scripts/objc3c_public_workflow_runner.py --list-json`

## Commands

| Package script | Runner action | Tier | Guarantee owner | Extra args | Backend |
| --- | --- | --- | --- | --- | --- |
| `build` | `build-default` | `-` | `-` | `fixed-shape` | `runner-internal` |
| `build:objc3c-native` | `build-native-binaries` | `-` | `-` | `fixed-shape` | `pwsh:scripts/build_objc3c_native.ps1` |
| `build:objc3c-native:contracts` | `build-native-contracts` | `-` | `-` | `fixed-shape` | `pwsh:scripts/build_objc3c_native.ps1` |
| `build:objc3c-native:full` | `build-native-full` | `-` | `-` | `fixed-shape` | `pwsh:scripts/build_objc3c_native.ps1` |
| `build:objc3c-native:reconfigure` | `build-native-reconfigure` | `-` | `-` | `fixed-shape` | `pwsh:scripts/build_objc3c_native.ps1` |
| `build:spec` | `build-spec` | `-` | `-` | `fixed-shape` | `python:scripts/build_site_index.py + npx prettier` |
| `compile:objc3c` | `compile-objc3c` | `-` | `-` | `pass-through` | `pwsh:scripts/objc3c_native_compile.ps1` |
| `lint:spec` | `lint-spec` | `-` | `-` | `fixed-shape` | `python:scripts/spec_lint.py` |
| `test` | `test-default` | `-` | `-` | `fixed-shape` | `runner-internal` |
| `test:fast` | `test-fast` | `fast` | `runtime acceptance, canonical replay, and a bounded smoke slice` | `fixed-shape` | `runner-internal + targeted smoke slice` |
| `test:smoke` | `test-smoke` | `smoke` | `execution smoke plus runtime acceptance` | `fixed-shape` | `runner-internal` |
| `test:ci` | `test-ci` | `ci` | `task hygiene plus full developer validation` | `fixed-shape` | `runner-internal + direct task hygiene` |
| `test:objc3c` | `test-recovery` | `recovery` | `recovery compile success and deterministic recovery diagnostics` | `pass-through` | `pwsh:scripts/check_objc3c_native_recovery_contract.ps1` |
| `test:objc3c:execution-smoke` | `test-execution-smoke` | `smoke` | `compile/link/run execution behavior` | `pass-through` | `pwsh:scripts/check_objc3c_native_execution_smoke.ps1` |
| `test:objc3c:execution-replay-proof` | `test-execution-replay` | `full` | `replay and native-output truth` | `pass-through` | `pwsh:scripts/check_objc3c_execution_replay_proof.ps1` |
| `test:objc3c:runtime-acceptance` | `test-runtime-acceptance` | `fast` | `runtime acceptance and ABI/accessor proof` | `fixed-shape` | `python:scripts/check_objc3c_runtime_acceptance.py` |
| `test:objc3c:fixture-matrix` | `test-fixture-matrix` | `nightly` | `broad positive corpus artifact sanity` | `pass-through` | `pwsh:scripts/run_objc3c_native_fixture_matrix.ps1` |
| `test:objc3c:negative-expectations` | `test-negative-expectations` | `nightly` | `negative expectation header and token enforcement` | `pass-through` | `pwsh:scripts/check_objc3c_negative_fixture_expectations.ps1` |
| `test:objc3c:full` | `test-full` | `full` | `smoke, recovery, runtime acceptance, and replay` | `fixed-shape` | `runner-internal + direct PowerShell suites` |
| `test:objc3c:nightly` | `test-nightly` | `nightly` | `full validation plus positive matrix and expectation enforcement` | `fixed-shape` | `runner-internal + direct PowerShell suites` |
| `package:objc3c-native:runnable-toolchain` | `package-runnable-toolchain` | `-` | `-` | `fixed-shape` | `pwsh:scripts/package_objc3c_runnable_toolchain.ps1` |
| `proof:objc3c` | `proof-objc3c` | `-` | `-` | `fixed-shape` | `pwsh:scripts/run_objc3c_native_compile_proof.ps1` |

## Operator Notes

- Use the package scripts above for normal operator workflows.
- `compile:objc3c` and the fixture-backed suite commands accept pass-through arguments for bounded selectors.
- No additional package-script compatibility aliases remain supported.
- Maintainer-only package scripts are limited to repo hygiene and boundary checks.
