# M254 Driver, Link, and Runtime Launch Integration Edge-case and Compatibility Completion Expectations (D004)

Contract ID: `objc3c-runtime-launch-integration/m254-d004-v1`

`M254-D004` closes the remaining operator-path gap between emitted runtime
registration metadata and the scripts that compile, prove, link, and launch
native `.objc3` programs.

## Required launch contract

The emitted `module.runtime-registration-manifest.json` payload is the
authoritative launch contract for all runtime launch surfaces.

The contract must publish:

- `launch_integration_contract_id`
- `runtime_library_resolution_model`
- `driver_linker_flag_consumption_model`
- `compile_wrapper_command_surface`
- `compile_proof_command_surface`
- `execution_smoke_command_surface`
- `launch_integration_ready`

The canonical values are:

- contract id `objc3c-runtime-launch-integration/m254-d004-v1`
- runtime archive resolution model
  `registration-manifest-runtime-archive-path-is-authoritative`
- linker-flag consumption model
  `registration-manifest-driver-linker-flags-feed-proof-and-smoke-link-commands`
- compile wrapper `scripts/objc3c_native_compile.ps1`
- compile proof `scripts/run_objc3c_native_compile_proof.ps1`
- execution smoke `scripts/check_objc3c_native_execution_smoke.ps1`

## Required behavior

- `scripts/objc3c_native_compile.ps1` must validate the emitted runtime launch
  contract after a successful native compile.
- Cache restore in `scripts/objc3c_native_compile.ps1` must also validate the
  same emitted launch contract before reporting `cache_hit=true`.
- `scripts/run_objc3c_native_compile_proof.ps1` must use the compile wrapper,
  compare `module.runtime-registration-manifest.json` across deterministic
  replays, and publish launch-contract fields in `digest.json`.
- `scripts/check_objc3c_native_execution_smoke.ps1` must compile through the
  compile wrapper and must resolve runtime link inputs from the emitted launch
  contract rather than heuristic manifest fallback logic.
- The flattened `module.manifest.json` payload must mirror the same launch
  contract fields so the semantic surface and emitted artifact stay aligned.

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

## Validation and evidence

Validation must pass via:

- `python scripts/check_m254_d004_driver_link_and_runtime_launch_integration_edge_case_and_compatibility_completion.py`
- `python -m pytest tests/tooling/test_check_m254_d004_driver_link_and_runtime_launch_integration_edge_case_and_compatibility_completion.py -q`
- `python scripts/run_m254_d004_lane_d_readiness.py`

Evidence must be written to:

- `tmp/reports/m254/M254-D004/runtime_launch_integration_summary.json`
