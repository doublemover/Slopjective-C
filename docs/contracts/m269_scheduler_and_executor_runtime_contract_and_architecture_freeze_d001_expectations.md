# M269 Scheduler And Executor Runtime Contract And Architecture Freeze Expectations (D001)

Contract ID: `objc3c-part7-scheduler-executor-runtime-contract/m269-d001-v1`

Goal:
Freeze the truthful private Part 7 runtime boundary for scheduler-facing task helpers, executor tags, task-state publication, and cancellation observation.

Required behavior:

1. The helper ABI remains private to `objc3_runtime_bootstrap_internal.h` and does not widen `objc3_runtime.h`.
2. The helper cluster must include:
   - `objc3_runtime_spawn_task_i32`
   - `objc3_runtime_enter_task_group_scope_i32`
   - `objc3_runtime_add_task_group_task_i32`
   - `objc3_runtime_wait_task_group_next_i32`
   - `objc3_runtime_cancel_task_group_i32`
   - `objc3_runtime_task_is_cancelled_i32`
   - `objc3_runtime_task_on_cancel_i32`
   - `objc3_runtime_executor_hop_i32`
3. The runtime must publish one private testing snapshot:
   - `objc3_runtime_copy_task_runtime_state_for_testing`
4. Emitted IR must carry:
   - `; part7_scheduler_executor_runtime_contract = ...`
   - `!objc3.objc_part7_scheduler_executor_runtime_contract = !{!94}`
5. The positive fixture must compile and show the helper declarations plus the dedicated Part 7 runtime-contract boundary in emitted IR.
6. The runtime probe `tests/tooling/runtime/m269_d001_scheduler_executor_runtime_contract_probe.cpp` must prove the helper entrypoints execute and the testing snapshot records deterministic task, executor, and cancellation facts.
7. This issue must remain truthful: the public runtime header still does not widen, and broader live scheduler/runtime implementation remains later `M269-D002` work.

Evidence path:

- `tmp/reports/m269/M269-D001/scheduler_executor_runtime_contract_summary.json`
