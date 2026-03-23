# M269 Live Task Runtime And Executor Implementation Expectations (D002)

Contract ID: `objc3c-part7-live-task-runtime-integration/m269-d002-v1`

Goal:
Prove that the supported private Part 7 task helper cluster is a live runtime execution capability rather than only a frozen ABI surface.

Required behavior:

1. The helper ABI remains private to `objc3_runtime_bootstrap_internal.h` and does not widen `objc3_runtime.h`.
2. Emitted IR must carry:
   - `; part7_live_task_runtime_integration = ...`
   - `!objc3.objc_part7_live_task_runtime_integration = !{!95}`
3. The live runtime boundary must cover:
   - `objc3_runtime_spawn_task_i32`
   - `objc3_runtime_enter_task_group_scope_i32`
   - `objc3_runtime_add_task_group_task_i32`
   - `objc3_runtime_wait_task_group_next_i32`
   - `objc3_runtime_cancel_task_group_i32`
   - `objc3_runtime_task_is_cancelled_i32`
   - `objc3_runtime_task_on_cancel_i32`
   - `objc3_runtime_executor_hop_i32`
   - `objc3_runtime_copy_task_runtime_state_for_testing`
4. The runtime probe `tests/tooling/runtime/m269_d002_live_task_runtime_and_executor_implementation_probe.cpp` must prove deterministic helper execution and snapshot publication.
5. This issue must remain truthful: retained `O3S260` / `O3L300` front-door metadata-export gates may still block broader fixture shapes before IR/object emission.
6. The next issue remains `M269-D003` for cancellation/autorelease hardening.

Evidence path:

- `tmp/reports/m269/M269-D002/live_task_runtime_integration_summary.json`
