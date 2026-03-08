# M254-D004 Driver, Link, and Runtime Launch Integration Edge-case and Compatibility Completion Packet

Packet: `M254-D004`

## Objective

Close build-system and launch-path edge cases so bootstrap runs under the same
command surfaces used by compile, proof, and smoke scripts.

## Contract

- Contract id `objc3c-runtime-launch-integration/m254-d004-v1`
- Dependency `M254-D003`
- Lane `D`

## Required outputs

- emitted `module.runtime-registration-manifest.json` carries the D004 launch
  contract fields
- flattened `module.manifest.json` mirrors those launch contract fields
- compile wrapper validates emitted launch contract on both direct compile and
  cache restore
- compile proof validates deterministic `module.runtime-registration-manifest.json`
  output and records launch-contract facts in `digest.json`
- execution smoke compiles through the wrapper and resolves runtime link inputs
  from emitted `driver_linker_flags`

## Canonical launch-contract values

- `launch_integration_contract_id` = `objc3c-runtime-launch-integration/m254-d004-v1`
- `runtime_library_resolution_model` = `registration-manifest-runtime-archive-path-is-authoritative`
- `driver_linker_flag_consumption_model` = `registration-manifest-driver-linker-flags-feed-proof-and-smoke-link-commands`
- `compile_wrapper_command_surface` = `scripts/objc3c_native_compile.ps1`
- `compile_proof_command_surface` = `scripts/run_objc3c_native_compile_proof.ps1`
- `execution_smoke_command_surface` = `scripts/check_objc3c_native_execution_smoke.ps1`
- `launch_integration_ready` = `true`

## Code anchors

- `native/objc3c/src/pipeline/objc3_frontend_types.h`
- `native/objc3c/src/pipeline/objc3_frontend_artifacts.cpp`
- `native/objc3c/src/io/objc3_process.h`
- `native/objc3c/src/io/objc3_process.cpp`
- `native/objc3c/src/driver/objc3_objc3_path.cpp`
- `native/objc3c/src/libobjc3c_frontend/frontend_anchor.cpp`
- `scripts/objc3c_runtime_launch_contract.ps1`
- `scripts/objc3c_native_compile.ps1`
- `scripts/run_objc3c_native_compile_proof.ps1`
- `scripts/check_objc3c_native_execution_smoke.ps1`

## Spec anchors

- `docs/objc3c-native.md`
- `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
- `spec/MODULE_METADATA_AND_ABI_TABLES.md`
- `tests/tooling/runtime/README.md`

## Validation commands

- `check:objc3c:m254-d004-driver-link-and-runtime-launch-integration-edge-case-and-compatibility-completion`
- `test:tooling:m254-d004-driver-link-and-runtime-launch-integration-edge-case-and-compatibility-completion`
- `check:objc3c:m254-d004-lane-d-readiness`

## Evidence

- `tmp/reports/m254/M254-D004/runtime_launch_integration_summary.json`
