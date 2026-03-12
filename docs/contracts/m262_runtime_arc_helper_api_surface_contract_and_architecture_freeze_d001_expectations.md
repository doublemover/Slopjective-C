# M262 Runtime ARC Helper API Surface Contract And Architecture Freeze Expectations (D001)

Contract ID: `objc3c-runtime-arc-helper-api-surface-freeze/m262-d001-v1`
Issue: `#7203`
Milestone: `M262`
Lane: `D`

## Required Outcome

`M262-D001` freezes the truthful private runtime ARC helper API surface after
`M262-C004`.

The frozen boundary must now prove all of the following:

- the stable public runtime header remains registration, lookup, dispatch, and
  testing snapshots only
- lowered retain/release/autorelease/current-property/weak/autoreleasepool
  helper entrypoints remain private to `objc3_runtime_bootstrap_internal.h`
- emitted IR publishes the canonical `runtime_arc_helper_api_surface` summary
  and `!objc3.objc_runtime_arc_helper_api_surface`
- no dedicated public ARC helper ABI lands here

## Required Artifacts

- `tests/tooling/fixtures/native/m262_arc_property_interaction_positive.objc3`
- `tmp/artifacts/compilation/objc3c-native/m262/d001/positive/module.ll`
- `tmp/artifacts/compilation/objc3c-native/m262/d001/positive/module.obj`
- `tmp/artifacts/compilation/objc3c-native/m262/d001/autoreleasepool/module.ll`
- `tmp/artifacts/compilation/objc3c-native/m262/d001/autoreleasepool/module.obj`
- `tmp/reports/m262/M262-D001/runtime_arc_helper_api_surface_contract_summary.json`

## Required Runtime Surface

The frozen boundary must publish:

- `; runtime_arc_helper_api_surface = contract=objc3c-runtime-arc-helper-api-surface-freeze/m262-d001-v1`
- `!objc3.objc_runtime_arc_helper_api_surface = !{!83}`
- `objc3_runtime_retain_i32`
- `objc3_runtime_release_i32`
- `objc3_runtime_autorelease_i32`
- `objc3_runtime_read_current_property_i32`
- `objc3_runtime_write_current_property_i32`
- `objc3_runtime_exchange_current_property_i32`
- `objc3_runtime_load_weak_current_property_i32`
- `objc3_runtime_store_weak_current_property_i32`
- `objc3_runtime_push_autoreleasepool_scope`
- `objc3_runtime_pop_autoreleasepool_scope`

The positive probes must prove:

- the private helper symbols still lower into emitted IR/object artifacts
- helper-driven retain/release/autorelease remain visible on the ARC property
  interaction path
- private autoreleasepool push/pop remain visible on the retained ownership
  runtime path

## Validation Commands

- `python scripts/check_m262_d001_runtime_arc_helper_api_surface_contract_and_architecture_freeze.py`
- `python -m pytest tests/tooling/test_check_m262_d001_runtime_arc_helper_api_surface_contract_and_architecture_freeze.py -q`
- `python scripts/run_m262_d001_lane_d_readiness.py`
