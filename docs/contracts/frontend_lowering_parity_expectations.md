# Frontend Lowering Parity Harness Expectations (M142)

Contract ID: `objc3c-frontend-lowering-parity-contract/m142-v1`

## Scope

M142 hardens deterministic parity replay between native CLI execution and the extracted C API frontend execution path.

## Required Contract Surface

| Check ID | Requirement |
| --- | --- |
| `M142-HAR-01` | `scripts/check_objc3c_library_cli_parity.py` supports source-mode execution with both `--cli-bin` and `--c-api-bin` and compares diagnostics/manifest/IR/object outputs. |
| `M142-HAR-02` | `native/objc3c/src/tools/objc3c_frontend_c_api_runner.cpp` publishes deterministic summary metadata and forwards lowering controls (`--objc3-max-message-args`, `--objc3-runtime-dispatch-symbol`). |
| `M142-HAR-03` | Native build surfaces include the C API runner binary (`artifacts/bin/objc3c-frontend-c-api-runner.exe`) through both `native/objc3c/CMakeLists.txt` and `scripts/build_objc3c_native.ps1`. |
| `M142-HAR-04` | `package.json` wires `check:objc3c:library-cli-parity:source`, `test:objc3c:m142-lowering-parity`, and `check:compiler-closeout:m142`; `check:task-hygiene` includes `check:compiler-closeout:m142`. |
| `M142-HAR-05` | `docs/objc3c-native` source fragments (`10-cli`, `30-semantics`, `50-artifacts`, `60-tests`) and this contract doc describe the parity harness flow, expected artifacts, and object-backend notes. |

## Verification Commands

- `python scripts/check_m142_frontend_lowering_parity_contract.py`
- `npm run test:objc3c:m142-lowering-parity`
- `npm run check:compiler-closeout:m142`

## Drift Remediation

1. Restore missing parity harness source/docs/package snippets for CLI and C API execution parity.
2. Re-run `python scripts/check_m142_frontend_lowering_parity_contract.py`.
3. Re-run `npm run check:compiler-closeout:m142`.
