# M268-D002 Packet: Live Continuation And Scheduler-Boundary Implementation - Core Feature Implementation

Issue: `#7291`

## Objective

Implement the first live Part 7 runtime integration path for the private continuation helper cluster.

## Scope

- emit live helper calls around the supported direct-call `await` happy path
- preserve truthful non-goals: no real suspension frame layout, no async state machine, no general executor runtime
- prove the linked native object path executes through the helper cluster and records deterministic snapshot traffic

## Required truths

- emitted IR carries `; part7_live_continuation_runtime_integration = ...`
- emitted IR carries `!objc3.objc_part7_live_continuation_runtime_integration = !{!92}`
- supported direct-call `await` sites emit helper call sites for allocation, handoff, and resume
- the runtime probe links the emitted object artifact with `artifacts/lib/objc3_runtime.lib`
- linked execution proves deterministic helper traffic through `objc3_runtime_copy_async_continuation_state_for_testing`
- the supported path remains non-suspending and direct-call-only

## Validation

- `python scripts/ensure_objc3c_native_build.py --mode fast --reason m268-d002-readiness`
- `python scripts/build_objc3c_native_docs.py`
- `python scripts/check_m268_d002_live_continuation_and_scheduler_boundary_implementation_core_feature_implementation.py`
- `python -m pytest tests/tooling/test_check_m268_d002_live_continuation_and_scheduler_boundary_implementation_core_feature_implementation.py -q`
- `python scripts/run_m268_d002_lane_d_readiness.py`

## Dependency continuity

- `M268-D001` is the previous issue
- `M268-D003` is the next issue
