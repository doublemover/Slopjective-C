# M268-D001 Packet: Continuation And Runtime-Helper Contract - Contract And Architecture Freeze

Issue: `#7290`

## Objective

Freeze the first truthful private runtime helper ABI for Part 7 continuation allocation, resume, and executor handoff.

## Scope

- add one private bootstrap-internal helper cluster for logical continuation handles
- add one private testing snapshot so probes can validate the helper traffic deterministically
- publish the boundary through lowering summaries, emitted IR comments, and named metadata
- keep the current async lowering claim narrow and truthful: direct-call-only lowering remains the live executable slice

## Required truths

- the helper ABI remains private to `native/objc3c/src/runtime/objc3_runtime_bootstrap_internal.h`
- the stable public runtime header `native/objc3c/src/runtime/objc3_runtime.h` does not widen
- emitted IR carries `; part7_continuation_runtime_helper = ...`
- emitted IR carries `!objc3.objc_part7_continuation_runtime_helper = !{!91}`
- the positive fixture compiles and emits declarations for:
  - `objc3_runtime_allocate_async_continuation_i32`
  - `objc3_runtime_handoff_async_continuation_to_executor_i32`
  - `objc3_runtime_resume_async_continuation_i32`
- the runtime probe proves deterministic helper execution and snapshot publication
- this issue still does not claim live suspension-frame materialization, async state-machine execution, or executor scheduling in compiled async functions

## Validation

- `python scripts/ensure_objc3c_native_build.py --mode fast --reason m268-d001-readiness`
- `python scripts/build_objc3c_native_docs.py`
- `python scripts/check_m268_d001_continuation_and_runtime_helper_contract_and_architecture_freeze.py`
- `python -m pytest tests/tooling/test_check_m268_d001_continuation_and_runtime_helper_contract_and_architecture_freeze.py -q`
- `python scripts/run_m268_d001_lane_d_readiness.py`

## Dependency continuity

- `M268-C003` is the previous issue
- `M268-D002` is the next issue
- D001 freezes a real helper ABI and snapshot surface first; D002 can then decide how much of that helper cluster becomes live lowering/runtime integration
