# Objective-C 3 Public Command Surface

This runbook is generated from the live public workflow runner metadata.

- Budget maximum: `25`
- Current public script count: `17`
- Runner path: `scripts/objc3c_public_workflow_runner.py`
- Introspection command: `python scripts/objc3c_public_workflow_runner.py --list-json`

## Commands

| Package script | Runner action | Extra args | Backend |
| --- | --- | --- | --- |
| `build` | `build-default` | `fixed-shape` | `runner-internal` |
| `build:objc3c-native` | `build-native-binaries` | `fixed-shape` | `pwsh:scripts/build_objc3c_native.ps1` |
| `build:objc3c-native:contracts` | `build-native-contracts` | `fixed-shape` | `pwsh:scripts/build_objc3c_native.ps1` |
| `build:objc3c-native:full` | `build-native-full` | `fixed-shape` | `pwsh:scripts/build_objc3c_native.ps1` |
| `build:objc3c-native:reconfigure` | `build-native-reconfigure` | `fixed-shape` | `pwsh:scripts/build_objc3c_native.ps1` |
| `build:spec` | `build-spec` | `fixed-shape` | `python:scripts/build_site_index.py + npx prettier` |
| `compile:objc3c` | `compile-objc3c` | `pass-through` | `pwsh:scripts/objc3c_native_compile.ps1` |
| `lint:spec` | `lint-spec` | `fixed-shape` | `python:scripts/spec_lint.py` |
| `test` | `test-default` | `fixed-shape` | `runner-internal` |
| `test:smoke` | `test-smoke` | `fixed-shape` | `runner-internal` |
| `test:ci` | `test-ci` | `fixed-shape` | `runner-internal + npm + pytest` |
| `test:objc3c` | `test-recovery` | `fixed-shape` | `pwsh:scripts/check_objc3c_native_recovery_contract.ps1` |
| `test:objc3c:execution-smoke` | `test-execution-smoke` | `fixed-shape` | `pwsh:scripts/check_objc3c_native_execution_smoke.ps1` |
| `test:objc3c:execution-replay-proof` | `test-execution-replay` | `fixed-shape` | `pwsh:scripts/check_objc3c_execution_replay_proof.ps1` |
| `test:objc3c:full` | `test-full` | `fixed-shape` | `runner-internal + npm` |
| `package:objc3c-native:runnable-toolchain` | `package-runnable-toolchain` | `fixed-shape` | `pwsh:scripts/package_objc3c_runnable_toolchain.ps1` |
| `proof:objc3c` | `proof-objc3c` | `fixed-shape` | `pwsh:scripts/run_objc3c_native_compile_proof.ps1` |

## Operator Notes

- Use the package scripts above for normal operator workflows.
- `compile:objc3c` is the only public command that accepts pass-through arguments.
- All other package scripts remain compatibility or internal surfaces during cleanup and may change without notice.
