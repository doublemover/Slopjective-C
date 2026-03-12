# M262 ARC Helper Entrypoints, Weak Operations, And Autorelease-Return Runtime Support Core Feature Implementation Expectations (D002)

Contract ID: `objc3c-runtime-arc-helper-runtime-support/m262-d002-v1`
Issue: `#7204`
Milestone: `M262`
Lane: `D`

## Required Outcome

`M262-D002` must prove the private ARC helper surface frozen by `M262-D001`
is a live compiler/runtime capability for the supported ARC slice rather than a
summary-only boundary.

The supported `D002` runtime-support boundary must now prove all of the
following:

- ARC-generated weak current-property access lowers through the private runtime
  helper entrypoints and emits object code successfully.
- ARC-generated autorelease-return paths lower through the private runtime
  helper entrypoints, link against the native runtime library, and execute
  successfully.
- The runtime helper entrypoints remain private bootstrap-internal surface; no
  public ARC runtime header widening lands here.
- The proof model remains narrowly scoped to the supported ARC property/weak
  and autorelease-return slice rooted in `M260-D002`, `M262-C004`, and
  `M262-D001`.

## Required Artifacts

- `tests/tooling/fixtures/native/m262_arc_property_interaction_positive.objc3`
- `tests/tooling/fixtures/native/m262_arc_block_autorelease_return_positive.objc3`
- `tests/tooling/runtime/m262_d002_arc_helper_runtime_support_probe.cpp`
- `tmp/artifacts/compilation/objc3c-native/m262/d002/property/module.ll`
- `tmp/artifacts/compilation/objc3c-native/m262/d002/property/module.obj`
- `tmp/artifacts/compilation/objc3c-native/m262/d002/block-autorelease/module.ll`
- `tmp/artifacts/compilation/objc3c-native/m262/d002/block-autorelease/module.obj`
- `tmp/artifacts/compilation/objc3c-native/m262/d002/runtime-probe/m262_d002_arc_helper_runtime_support_probe.exe`
- `tmp/reports/m262/M262-D002/arc_helper_runtime_support_summary.json`

## Required Runtime Surface

The `D002` boundary must publish:

- `; runtime_arc_helper_runtime_support = contract=objc3c-runtime-arc-helper-runtime-support/m262-d002-v1`
- `!objc3.objc_runtime_arc_helper_runtime_support = !{!84}`
- `objc3_runtime_retain_i32`
- `objc3_runtime_release_i32`
- `objc3_runtime_autorelease_i32`
- `objc3_runtime_load_weak_current_property_i32`
- `objc3_runtime_store_weak_current_property_i32`
- `objc3_runtime_push_autoreleasepool_scope`
- `objc3_runtime_pop_autoreleasepool_scope`

The positive probes must prove:

- the ARC property fixture lowers weak current-property helper calls into IR
  and emits a non-empty object artifact
- the ARC property fixture keeps `currentValue` as `strong-owned` and
  `weakValue` as `weak` in emitted runtime metadata source records
- the ARC autorelease-return fixture lowers through
  `objc3_runtime_autorelease_i32`
- the dedicated runtime probe links against the native runtime library and
  proves retain/release/autoreleasepool helper execution successfully

## Validation Commands

- `python scripts/check_m262_d002_arc_helper_entrypoints_weak_operations_and_autorelease_return_runtime_support_core_feature_implementation.py`
- `python -m pytest tests/tooling/test_check_m262_d002_arc_helper_entrypoints_weak_operations_and_autorelease_return_runtime_support_core_feature_implementation.py -q`
- `python scripts/run_m262_d002_lane_d_readiness.py`
