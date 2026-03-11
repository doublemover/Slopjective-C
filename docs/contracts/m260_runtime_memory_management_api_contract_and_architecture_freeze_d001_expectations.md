# M260 Runtime Memory Management API Contract And Architecture Freeze Expectations (D001)

Contract ID: `objc3c-runtime-memory-management-api-freeze/m260-d001-v1`
Issue: `#7175`
Milestone: `M260`
Lane: `D`

## Required Outcome

`M260-D001` freezes the truthful runtime memory-management API boundary after
`M260-C002`.

The frozen boundary must now prove all of the following:

- the stable public runtime header remains register/lookup/dispatch plus
  testing snapshots
- lowered retain/release/autorelease/current-property/weak helper entrypoints
  remain private to `objc3_runtime_bootstrap_internal.h`
- emitted IR publishes the canonical `runtime_memory_management_api` summary
  and `!objc3.objc_runtime_memory_management_api`
- no public autoreleasepool push/pop runtime API lands here

## Required Artifacts

- `tests/tooling/fixtures/native/m260_ownership_runtime_hook_emission_positive.objc3`
- `tests/tooling/runtime/m260_d001_runtime_memory_management_api_probe.cpp`
- `tmp/artifacts/compilation/objc3c-native/m260/d001/positive/module.ll`
- `tmp/artifacts/compilation/objc3c-native/m260/d001/positive/module.obj`
- `tmp/reports/m260/M260-D001/runtime_memory_management_api_contract_summary.json`

## Required Runtime Surface

The frozen boundary must publish:

- `; runtime_memory_management_api = contract=objc3c-runtime-memory-management-api-freeze/m260-d001-v1`
- `!objc3.objc_runtime_memory_management_api = !{!70}`
- `objc3_runtime_retain_i32`
- `objc3_runtime_release_i32`
- `objc3_runtime_autorelease_i32`
- `objc3_runtime_read_current_property_i32`
- `objc3_runtime_write_current_property_i32`
- `objc3_runtime_exchange_current_property_i32`
- `objc3_runtime_load_weak_current_property_i32`
- `objc3_runtime_store_weak_current_property_i32`

The runtime probe must prove:

- the private helper symbols link and execute against the current runtime
- the strong property keeps the child alive through helper-driven retain and
  release traffic
- the weak property zeroes after the strong edge is cleared
- releasing the parent tears down the final live runtime instance

## Validation Commands

- `python scripts/check_m260_d001_runtime_memory_management_api_contract_and_architecture_freeze.py`
- `python -m pytest tests/tooling/test_check_m260_d001_runtime_memory_management_api_contract_and_architecture_freeze.py -q`
- `python scripts/run_m260_d001_lane_d_readiness.py`
