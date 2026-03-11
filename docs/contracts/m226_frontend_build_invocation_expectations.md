# Frontend Build and Invocation Integration Expectations (M226-D001)

Contract ID: `objc3c-frontend-build-invocation-contract/m226-d001-v1`

## Scope

M226-D001 freezes frontend build and invocation integration so native entrypoints remain deterministic, diagnostics stay explicit, and backend routing cannot drift into hidden fallback behavior.

## Required Contract Surface

| Check ID       | Requirement |
| -------------- | ----------- |
| `M226-BLD-01`  | `scripts/build_objc3c_native.ps1` deterministically builds both `artifacts/bin/objc3c-native.exe` and `artifacts/bin/objc3c-frontend-c-api-runner.exe`, stages transient outputs under `tmp/build-objc3c-native`, and keeps shared frontend/driver source wiring explicit. |
| `M226-BLD-02`  | `scripts/objc3c_native_compile.ps1` keeps deterministic invocation flow through `scripts/build_objc3c_native.ps1` and `artifacts/bin/objc3c-native.exe`, with cache/state artifacts rooted under `tmp/artifacts/objc3c-native/cache`. |
| `M226-INV-01`  | `native/objc3c/src/driver/objc3_driver_main.cpp` keeps parse -> capability routing -> compilation dispatch ordering and fail-closed `stderr` diagnostics with exit code `2` on route/parse failures. |
| `M226-INV-02`  | `native/objc3c/src/driver/objc3_compilation_driver.cpp` and `native/objc3c/src/driver/objc3_driver_shell.cpp` preserve deterministic input classification plus explicit missing-input/clang/llc diagnostics before dispatch. |
| `M226-INV-03`  | `native/objc3c/src/driver/objc3_objc3_path.cpp` keeps explicit clang vs llvm-direct object backend invocation branches, surfaces backend errors, and emits deterministic backend marker artifacts. |
| `M226-INV-04`  | `native/objc3c/src/driver/objc3_llvm_capability_routing.cpp` fail-closes when capability summary data is missing/invalid and deterministically maps backend routing from `llc --filetype=obj` capability. |
| `M226-INV-05`  | `native/objc3c/src/driver/objc3_cli_options.cpp` continues exposing explicit invocation switches (`--objc3-ir-object-backend`, `--llvm-capabilities-summary`, `--objc3-route-backend-from-capabilities`) and deterministic invalid-backend diagnostics. |
| `M226-INV-06`  | Frontend invocation contract remains ambiguity-free: no hidden compile fallback callsites are allowed in capability-routing or objc3 invocation path surfaces. |
| `M226-GATE-01` | `python scripts/check_m226_d001_frontend_build_invocation_contract.py` and `python -m pytest tests/tooling/test_check_m226_d001_frontend_build_invocation_contract.py -q` remain the fail-closed freeze validators for M226-D001. |

## Public command surface`r`n`r`n- `npm run build:objc3c-native` for fast native binary publication`r`n- `npm run build:objc3c-native:contracts` for scaffold/invocation-lock/core-expansion packet regeneration`r`n- `npm run build:objc3c-native:full` for closeout/conformance packet regeneration and full-path proofs`r`n`r`n## Verification Commands

- `python scripts/check_m226_d001_frontend_build_invocation_contract.py`
- `python -m pytest tests/tooling/test_check_m226_d001_frontend_build_invocation_contract.py -q`

## Drift Remediation

1. Restore M226-D001 deterministic build/invocation snippets across scripts and driver sources.
2. Re-run `python scripts/check_m226_d001_frontend_build_invocation_contract.py`.
3. Re-run `python -m pytest tests/tooling/test_check_m226_d001_frontend_build_invocation_contract.py -q`.

