# M269 Task Runtime Lowering Contract And Architecture Freeze Expectations (C001)

Contract ID: `objc3c-part7-task-runtime-lowering-contract/m269-c001-v1`

Frontend lowering-surface path:

- `frontend.pipeline.semantic_surface.objc_part7_task_runtime_lowering_contract`

Required truths:

- the already-landed `M269-B001`, `M269-B002`, and `M269-B003` task semantic surfaces now lower into explicit replay-stable task runtime contracts instead of leaving the existing Part 7 IR metadata channels empty
- the positive fixture proves deterministic lowering counts for task creation, task-group artifacts, executor-hop affinity, cancellation polls, and scheduler-visible replay guard points
- emitted IR continues to carry the dedicated Part 7 named metadata anchors for actor isolation/sendability, task runtime interop/cancellation, and concurrency replay/race guards
- this issue still does not claim live task spawning, executor-hop runtime execution, scheduler integration, or completed task-group ABI/runtime behavior

Required probes:

- `python scripts/ensure_objc3c_native_build.py --mode fast --reason m269-c001-readiness`
- `artifacts/bin/objc3c-frontend-c-api-runner.exe tests/tooling/fixtures/native/m269_c001_task_runtime_lowering_positive.objc3 --out-dir tmp/artifacts/compilation/objc3c-native/m269/c001/positive --emit-prefix module --no-emit-ir --no-emit-object`

Expected positive summary facts:

- `task_creation_sites = 2`
- `task_group_scope_sites = 1`
- `task_group_add_task_sites = 1`
- `task_group_wait_next_sites = 1`
- `task_group_cancel_all_sites = 1`
- `detached_task_creation_sites = 1`
- `actor_isolation_sites = 2`
- `cross_actor_hop_sites = 1`
- `task_runtime_sites = 14`
- `task_runtime_interop_sites = 14`
- `cancellation_probe_sites = 4`
- `cancellation_handler_sites = 1`
- `runtime_resume_sites = 1`
- `runtime_cancel_sites = 1`
- `concurrency_replay_sites = 14`
- `replay_proof_sites = 3`
- `race_guard_sites = 5`
- `task_handoff_sites = 4`
- `deterministic_schedule_sites = 14`
- `deterministic_handoff = true`
- `ready_for_ir_emission = true`

Expected static IR proof:

- IR contains `; actor_isolation_sendability_lowering = actor_isolation_sites=2`
- IR contains `; task_runtime_interop_cancellation_lowering = task_runtime_sites=14`
- IR contains `; concurrency_replay_race_guard_lowering = concurrency_replay_sites=14`
- IR contains `; frontend_objc_actor_isolation_sendability_lowering_profile = actor_isolation_sites=2`
- IR contains `; frontend_objc_task_runtime_interop_cancellation_lowering_profile = task_runtime_sites=14`
- IR contains `; frontend_objc_concurrency_replay_race_guard_lowering_profile = concurrency_replay_sites=14`
- IR contains `!objc3.objc_concurrency_replay_race_guard_lowering = !{!39}`
- IR contains `!objc3.objc_task_runtime_interop_cancellation_lowering = !{!40}`
- IR contains `!objc3.objc_actor_isolation_sendability_lowering = !{!41}`
