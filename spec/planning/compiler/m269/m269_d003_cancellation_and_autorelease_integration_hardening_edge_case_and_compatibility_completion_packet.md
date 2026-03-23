# M269-D003 Packet: Cancellation And Autorelease Integration Hardening - Edge-Case And Compatibility Completion

Issue: `#7304`

## Objective

Harden the live private Part 7 task-runtime boundary around cancellation cleanup, autoreleasepool scopes, and reset-stable replay determinism.

## Scope

- add one canonical hardening summary above the live `M269-D002` runtime boundary
- publish that boundary in emitted IR comments and named metadata
- keep the helper/runtime ABI private to the runtime internal header
- prove deterministic two-pass replay through reset, task snapshot, memory snapshot, ARC-debug snapshot, and autoreleasepool push/pop boundaries
- keep the claim truthful: this does not widen the public runtime header or unblock broader front-door metadata-export gates

## Required truths

- emitted IR carries `; part7_task_runtime_hardening = ...`
- emitted IR carries `!objc3.objc_part7_task_runtime_hardening = !{!96}`
- the probe validates:
  - `objc3_runtime_reset_for_testing`
  - `objc3_runtime_copy_task_runtime_state_for_testing`
  - `objc3_runtime_copy_memory_management_state_for_testing`
  - `objc3_runtime_copy_arc_debug_state_for_testing`
  - `objc3_runtime_push_autoreleasepool_scope`
  - `objc3_runtime_pop_autoreleasepool_scope`
- task helper results and snapshots remain deterministic across two identical passes separated by `objc3_runtime_reset_for_testing`
- the public runtime header still does not widen
- broader front-door metadata-export gating remains outside this issue
- `M269-E001` is the next issue

## Validation

- `python scripts/build_objc3c_native_docs.py`
- `python scripts/check_m269_d003_cancellation_and_autorelease_integration_hardening_edge_case_and_compatibility_completion.py --skip-dynamic-probes`
- `python scripts/check_m269_d003_cancellation_and_autorelease_integration_hardening_edge_case_and_compatibility_completion.py`
- `python -m pytest tests/tooling/test_check_m269_d003_cancellation_and_autorelease_integration_hardening_edge_case_and_compatibility_completion.py -q`
- `python scripts/run_m269_d003_lane_d_readiness.py`

## Dependency continuity

- `M269-D002` proves the live helper-backed task runtime boundary
- `M269-D003` hardens that same boundary around reset/autorelease/cancellation determinism
- `M269-E001` closes the lane-E milestone gate on top of this hardened runtime slice
