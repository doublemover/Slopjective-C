# M260 Runtime Hook Emission For Retain, Release, Autorelease, And Weak Paths Core Feature Implementation Expectations (C002)

Contract ID: `objc3c-ownership-runtime-hook-emission/m260-c002-v1`
Issue: `#7174`
Milestone: `M260`
Lane: `C`

## Required Outcome

`M260-C002` replaces the old summary-only ownership lowering baseline with live runtime hook emission for synthesized runtime-backed property accessors.

The emitted accessor surface must now prove all of the following:

- synthesized strong-object getters call runtime-owned read, retain, and autorelease helpers
- synthesized strong-object setters call runtime-owned retain, exchange, and release helpers
- synthesized weak-object getters and setters call runtime-owned weak helpers
- the runtime consumes those helpers against the current dispatch-frame property context and realized per-instance storage
- legacy synthesized storage globals remain emitted so previously-closed `M257-C003` artifact contracts do not drift

## Required Artifacts

- `tests/tooling/fixtures/native/m260_ownership_runtime_hook_emission_positive.objc3`
- `tests/tooling/runtime/m260_c002_ownership_runtime_hook_probe.cpp`
- `tmp/artifacts/compilation/objc3c-native/m260/c002/positive/module.ll`
- `tmp/artifacts/compilation/objc3c-native/m260/c002/positive/module.obj`
- `tmp/reports/m260/M260-C002/ownership_runtime_hook_emission_summary.json`

## Required IR / Runtime Surface

The emitted IR must publish:

- `; ownership_runtime_hook_emission = contract=objc3c-ownership-runtime-hook-emission/m260-c002-v1`
- `!objc3.objc_runtime_ownership_hook_emission = !{!69}`
- `@objc3_runtime_read_current_property_i32`
- `@objc3_runtime_write_current_property_i32`
- `@objc3_runtime_exchange_current_property_i32`
- `@objc3_runtime_load_weak_current_property_i32`
- `@objc3_runtime_store_weak_current_property_i32`
- `@objc3_runtime_retain_i32`
- `@objc3_runtime_release_i32`
- `@objc3_runtime_autorelease_i32`

The runtime probe must prove:

- a strong property retain keeps a child object alive after the local reference is released
- a strong getter returns the child identity on the live runtime path
- clearing the strong property releases the child and zeroes the weak property
- releasing the parent tears down the final live runtime instance

## Validation Commands

- `python scripts/check_m260_c002_runtime_hook_emission_for_retain_release_autorelease_and_weak_paths_core_feature_implementation.py`
- `python -m pytest tests/tooling/test_check_m260_c002_runtime_hook_emission_for_retain_release_autorelease_and_weak_paths_core_feature_implementation.py -q`
- `python scripts/run_m260_c002_lane_c_readiness.py`
