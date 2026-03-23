# M269 Cancellation And Autorelease Integration Hardening Expectations (D003)

Contract ID: `objc3c-part7-task-runtime-hardening/m269-d003-v1`

Goal:
Prove that the live private Part 7 task-runtime boundary remains deterministic across cancellation edges, autoreleasepool scopes, and explicit runtime resets.

Required behavior:

1. The helper/runtime surface remains private and does not widen `objc3_runtime.h`.
2. Emitted IR must carry:
   - `; part7_task_runtime_hardening = ...`
   - `!objc3.objc_part7_task_runtime_hardening = !{!96}`
3. The runtime probe `tests/tooling/runtime/m269_d003_task_runtime_hardening_probe.cpp` must validate deterministic two-pass replay across:
   - `objc3_runtime_reset_for_testing`
   - `objc3_runtime_copy_task_runtime_state_for_testing`
   - `objc3_runtime_copy_memory_management_state_for_testing`
   - `objc3_runtime_copy_arc_debug_state_for_testing`
   - `objc3_runtime_push_autoreleasepool_scope`
   - `objc3_runtime_pop_autoreleasepool_scope`
4. Cancellation/task helper results must remain stable across the replayed passes.
5. This issue must remain truthful: broader front-door metadata-export gating and general public task-runtime ABI widening are still deferred.
6. The next issue remains `M269-E001`.

Evidence path:

- `tmp/reports/m269/M269-D003/task_runtime_hardening_summary.json`
