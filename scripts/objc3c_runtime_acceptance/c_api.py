"""Runtime C API and private test ABI boundaries for acceptance."""

from __future__ import annotations

RUNTIME_PUBLIC_HEADER_PATH = "native/objc3c/src/runtime/public/objc3_runtime_api.h"
RUNTIME_BOOTSTRAP_INTERNAL_HEADER_PATH = (
    "native/objc3c/src/runtime/objc3_runtime_bootstrap_internal.h"
)

PUBLIC_RUNTIME_ABI_BOUNDARY = [
    "objc3_runtime_register_image",
    "objc3_runtime_lookup_selector",
    "objc3_runtime_dispatch_i32",
    "objc3_runtime_reset_for_testing",
]
RUNTIME_INSTALLATION_ABI_BOUNDARY = [
    "objc3_runtime_register_image",
    "objc3_runtime_copy_registration_state_for_testing",
    "objc3_runtime_reset_for_testing",
]
RUNTIME_LOADER_TESTING_BOUNDARY = [
    "objc3_runtime_stage_registration_table_for_bootstrap",
    "objc3_runtime_copy_image_walk_state_for_testing",
    "objc3_runtime_replay_registered_images_for_testing",
    "objc3_runtime_copy_reset_replay_state_for_testing",
]
PRIVATE_RELEASE_CANDIDATE_CLAIM_RUNTIME_ABI_BOUNDARY = [
    "objc3_runtime_copy_release_candidate_claim_snapshot_for_testing",
]
PRIVATE_RELEASE_CANDIDATE_EVIDENCE_RUNTIME_BOUNDARY = [
    "objc3_runtime_copy_release_candidate_evidence_state_for_testing",
]
PRIVATE_BLOCK_ARC_RUNTIME_ABI_BOUNDARY = [
    "objc3_runtime_promote_block_i32",
    "objc3_runtime_invoke_block_i32",
    "objc3_runtime_retain_i32",
    "objc3_runtime_release_i32",
    "objc3_runtime_autorelease_i32",
    "objc3_runtime_push_autoreleasepool_scope",
    "objc3_runtime_pop_autoreleasepool_scope",
    "objc3_runtime_read_current_property_i32",
    "objc3_runtime_write_current_property_i32",
    "objc3_runtime_exchange_current_property_i32",
    "objc3_runtime_bind_current_property_context_for_testing",
    "objc3_runtime_clear_current_property_context_for_testing",
    "objc3_runtime_load_weak_current_property_i32",
    "objc3_runtime_store_weak_current_property_i32",
    "objc3_runtime_copy_arc_debug_state_for_testing",
    "objc3_runtime_copy_block_arc_runtime_abi_snapshot_for_testing",
]
PRIVATE_ERROR_RUNTIME_ABI_BOUNDARY = [
    "objc3_runtime_store_thrown_error_i32",
    "objc3_runtime_load_thrown_error_i32",
    "objc3_runtime_bridge_status_error_i32",
    "objc3_runtime_bridge_nserror_error_i32",
    "objc3_runtime_catch_matches_error_i32",
    "objc3_runtime_copy_error_bridge_state_for_testing",
]
PRIVATE_UNIFIED_CONCURRENCY_RUNTIME_ABI_BOUNDARY = [
    "objc3_runtime_allocate_async_continuation_i32",
    "objc3_runtime_handoff_async_continuation_to_executor_i32",
    "objc3_runtime_resume_async_continuation_i32",
    "objc3_runtime_cancel_async_continuation_i32",
    "objc3_runtime_spawn_task_i32",
    "objc3_runtime_enter_task_group_scope_i32",
    "objc3_runtime_add_task_group_task_i32",
    "objc3_runtime_wait_task_group_next_i32",
    "objc3_runtime_cancel_task_group_i32",
    "objc3_runtime_task_is_cancelled_i32",
    "objc3_runtime_task_on_cancel_i32",
    "objc3_runtime_executor_hop_i32",
    "objc3_runtime_actor_enter_isolation_thunk_i32",
    "objc3_runtime_actor_enter_nonisolated_i32",
    "objc3_runtime_actor_hop_to_executor_i32",
    "objc3_runtime_actor_record_replay_proof_i32",
    "objc3_runtime_actor_record_race_guard_i32",
    "objc3_runtime_actor_bind_executor_i32",
    "objc3_runtime_actor_mailbox_enqueue_i32",
    "objc3_runtime_actor_mailbox_drain_next_i32",
    "objc3_runtime_copy_async_continuation_state_for_testing",
    "objc3_runtime_copy_task_runtime_state_for_testing",
    "objc3_runtime_copy_actor_runtime_state_for_testing",
]

UNIFIED_CONCURRENCY_RUNTIME_ABI_BOUNDARY_MODEL = (
    "private-async-task-and-actor-helper-entrypoints-plus-testing-snapshots-define-the-live-runtime-abi-without-widening-the-public-runtime-header"
)
UNIFIED_CONCURRENCY_CONTINUATION_RUNTIME_MODEL = (
    "continuation-allocation-handoff-resume-and-testing-snapshots-stay-on-bootstrap-internal-runtime-entrypoints"
)
UNIFIED_CONCURRENCY_TASK_RUNTIME_MODEL = (
    "task-spawn-group-cancellation-executor-hop-and-testing-snapshots-stay-on-bootstrap-internal-runtime-entrypoints"
)
UNIFIED_CONCURRENCY_ACTOR_RUNTIME_MODEL = (
    "actor-isolation-nonisolated-hop-replay-race-guard-mailbox-executor-binding-failure-codes-and-testing-snapshots-stay-on-bootstrap-internal-runtime-entrypoints"
)
UNIFIED_CONCURRENCY_RUNTIME_FAIL_CLOSED_MODEL = (
    "public-runtime-header-remains-registration-lookup-dispatch-only-until-deliberate-concurrency-runtime-abi-widening"
)
BLOCK_ARC_RUNTIME_ABI_BOUNDARY_MODEL = (
    "private-block-and-arc-helper-entrypoints-plus-testing-snapshots-define-the-live-runtime-abi-without-widening-the-public-runtime-header"
)
BLOCK_ARC_RUNTIME_BLOCK_MODEL = (
    "descriptor-backed-promote-invoke-and-handle-lifetime-for-supported-block-records-stay-on-bootstrap-internal-runtime-entrypoints"
)
BLOCK_ARC_RUNTIME_DESCRIPTOR_MODEL = (
    "storage-slot-zero-carries-an-internal-descriptor-pointer-whose-record-preserves-size-captures-flags-arity-and-invoke-thunk"
)
BLOCK_ARC_RUNTIME_INVOKE_THUNK_MODEL = (
    "runtime-invocation-plans-call-the-descriptor-owned-i32-invoke-thunk-with-copied-runtime-owned-storage"
)
BLOCK_ARC_RUNTIME_ARC_MODEL = (
    "retain-release-autorelease-autoreleasepool-and-current-property-weak-helper-traffic-stays-on-bootstrap-internal-runtime-entrypoints"
)
BLOCK_ARC_RUNTIME_FAIL_CLOSED_MODEL = (
    "public-runtime-header-remains-registration-lookup-dispatch-only-until-deliberate-runtime-abi-widening"
)

__all__ = [name for name in globals() if name.isupper()]
