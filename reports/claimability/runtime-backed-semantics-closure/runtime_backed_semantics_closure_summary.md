# Runtime-Backed Semantics Closure

- Issue: `#8017`
- Contract: `objc3c.runtime.backed.semantics.closure.v1`
- Status: `PASS`
- Positive executable fixtures: `7`
- Negative fail-closed fixtures: `4`
- Private runtime helper symbols: `36`
- Durable replay fixture directories: `13`
- Scratch output: `tmp/artifacts/objc3c-native/runtime-backed-semantics-closure` (not source truth)

## Checks

- `positive_compile`: `true`
- `negative_compile`: `true`
- `required_ir_tokens`: `true`
- `runtime_helper_ir_declarations`: `true`
- `source_tokens`: `true`
- `durable_replay_dirs`: `true`
- `stress_manifest`: `true`
- `conformance`: `true`
- `no_source_truth_under_tmp`: `true`

## Fixture Coverage

- `block_arc_autorelease_return`: `tests/tooling/fixtures/native/arc_block_autorelease_return_positive.objc3` compiled=`true`
- `arc_weak_autoreleasepool`: `tests/tooling/fixtures/native/reference_counting_weak_autoreleasepool_positive.objc3` compiled=`true`
- `error_runtime_bridge_helper`: `tests/tooling/fixtures/native/error_runtime_bridge_helper_positive.objc3` compiled=`true`
- `live_error_runtime`: `tests/tooling/fixtures/native/live_error_runtime_integration_positive.objc3` compiled=`true`
- `live_continuation_runtime`: `tests/tooling/fixtures/native/live_continuation_runtime_integration_positive.objc3` compiled=`true`
- `live_task_runtime`: `tests/tooling/fixtures/native/live_task_runtime_and_executor_implementation_positive.objc3` compiled=`true`
- `live_actor_mailbox_runtime`: `tests/tooling/fixtures/native/live_actor_mailbox_runtime_positive.objc3` compiled=`true`
- `escaping_block_actor_handoff`: `tests/tooling/fixtures/native/escaping_block_with_handoff_rejected.objc3` rejected=`true` codes=`O3S294`
- `throw_without_throws_or_catch`: `tests/tooling/fixtures/native/throw_requires_throws_or_catch_negative.objc3` rejected=`true` codes=`O3S274`
- `task_group_without_scope`: `tests/tooling/fixtures/native/task_group_without_scope_rejected.objc3` rejected=`true` codes=`O3S228,O3S231`
- `actor_hop_without_async`: `tests/tooling/fixtures/native/actor_hop_without_async_rejected.objc3` rejected=`true` codes=`O3S289`

## Runtime Helper Boundary

- `objc3_runtime_read_current_property_i32`: ir_declared=`true`
- `objc3_runtime_write_current_property_i32`: ir_declared=`true`
- `objc3_runtime_exchange_current_property_i32`: ir_declared=`true`
- `objc3_runtime_store_thrown_error_i32`: ir_declared=`true`
- `objc3_runtime_load_thrown_error_i32`: ir_declared=`true`
- `objc3_runtime_bridge_status_error_i32`: ir_declared=`true`
- `objc3_runtime_bridge_nserror_error_i32`: ir_declared=`true`
- `objc3_runtime_catch_matches_error_i32`: ir_declared=`true`
- `objc3_runtime_allocate_async_continuation_i32`: ir_declared=`true`
- `objc3_runtime_handoff_async_continuation_to_executor_i32`: ir_declared=`true`
- `objc3_runtime_resume_async_continuation_i32`: ir_declared=`true`
- `objc3_runtime_spawn_task_i32`: ir_declared=`true`
- `objc3_runtime_enter_task_group_scope_i32`: ir_declared=`true`
- `objc3_runtime_add_task_group_task_i32`: ir_declared=`true`
- `objc3_runtime_wait_task_group_next_i32`: ir_declared=`true`
- `objc3_runtime_cancel_task_group_i32`: ir_declared=`true`
- `objc3_runtime_task_is_cancelled_i32`: ir_declared=`true`
- `objc3_runtime_task_on_cancel_i32`: ir_declared=`true`
- `objc3_runtime_executor_hop_i32`: ir_declared=`true`
- `objc3_runtime_actor_enter_isolation_thunk_i32`: ir_declared=`true`
- `objc3_runtime_actor_enter_nonisolated_i32`: ir_declared=`true`
- `objc3_runtime_actor_hop_to_executor_i32`: ir_declared=`true`
- `objc3_runtime_actor_record_replay_proof_i32`: ir_declared=`true`
- `objc3_runtime_actor_record_race_guard_i32`: ir_declared=`true`
- `objc3_runtime_actor_bind_executor_i32`: ir_declared=`true`
- `objc3_runtime_actor_mailbox_enqueue_i32`: ir_declared=`true`
- `objc3_runtime_actor_mailbox_drain_next_i32`: ir_declared=`true`
- `objc3_runtime_load_weak_current_property_i32`: ir_declared=`true`
- `objc3_runtime_store_weak_current_property_i32`: ir_declared=`true`
- `objc3_runtime_retain_i32`: ir_declared=`true`
- `objc3_runtime_release_i32`: ir_declared=`true`
- `objc3_runtime_autorelease_i32`: ir_declared=`true`
- `objc3_runtime_promote_block_i32`: ir_declared=`true`
- `objc3_runtime_invoke_block_i32`: ir_declared=`true`
- `objc3_runtime_push_autoreleasepool_scope`: ir_declared=`true`
- `objc3_runtime_pop_autoreleasepool_scope`: ir_declared=`true`
