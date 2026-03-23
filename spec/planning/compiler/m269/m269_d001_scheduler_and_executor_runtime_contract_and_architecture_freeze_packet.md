# M269-D001 Packet: Scheduler And Executor Runtime Contract - Contract And Architecture Freeze

Issue: `#7302`

## Objective

Freeze the truthful private Part 7 runtime boundary for scheduler-facing task helpers, executor tags, task-state publication, and cancellation observation.

## Scope

- freeze the existing private task helper cluster and task-state snapshot as one runtime contract
- publish the boundary through lowering summaries, emitted IR comments, and named metadata
- keep the public runtime header unchanged
- keep the claim truthful: this is not yet the broad live scheduler/runtime implementation issue

## Required truths

- the helper ABI remains private to `native/objc3c/src/runtime/objc3_runtime_bootstrap_internal.h`
- the stable public runtime header `native/objc3c/src/runtime/objc3_runtime.h` does not widen
- emitted IR carries `; part7_scheduler_executor_runtime_contract = ...`
- emitted IR carries `!objc3.objc_part7_scheduler_executor_runtime_contract = !{!94}`
- the helper cluster includes:
  - `objc3_runtime_spawn_task_i32`
  - `objc3_runtime_enter_task_group_scope_i32`
  - `objc3_runtime_add_task_group_task_i32`
  - `objc3_runtime_wait_task_group_next_i32`
  - `objc3_runtime_cancel_task_group_i32`
  - `objc3_runtime_task_is_cancelled_i32`
  - `objc3_runtime_task_on_cancel_i32`
  - `objc3_runtime_executor_hop_i32`
- the private testing snapshot remains `objc3_runtime_copy_task_runtime_state_for_testing`
- the runtime probe proves deterministic helper execution and snapshot publication
- broader live scheduler and executor implementation remains the next issue: `M269-D002`

## Validation

- `python scripts/ensure_objc3c_native_build.py --mode fast --reason m269-d001-readiness`
- `python scripts/build_objc3c_native_docs.py`
- `python scripts/check_m269_d001_scheduler_and_executor_runtime_contract_and_architecture_freeze.py`
- `python -m pytest tests/tooling/test_check_m269_d001_scheduler_and_executor_runtime_contract_and_architecture_freeze.py -q`
- `python scripts/run_m269_d001_lane_d_readiness.py`

## Dependency continuity

- `M269-C003` is the previous issue
- `M269-D002` is the next issue
- D001 freezes the private task/executor runtime contract first; D002 can then widen it into the broader live scheduler/runtime implementation surface
