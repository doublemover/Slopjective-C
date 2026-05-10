from __future__ import annotations

CONTRACT_ID = "objc3c.effects.ownership.semantic.model.closure.v1"
ISSUE = "#8014"

SUMMARY_FIELDS = [
    "arc_ownership_qualified_sites",
    "retain_insertion_sites",
    "release_insertion_sites",
    "autorelease_insertion_sites",
    "weak_zeroing_sites",
    "unowned_reference_sites",
    "weak_unowned_conflict_sites",
    "autoreleasepool_scope_sites",
    "cleanup_order_exit_sites",
    "block_literal_sites",
    "stack_to_heap_promotion_sites",
    "byref_forwarding_cell_sites",
    "copy_helper_required_sites",
    "dispose_helper_required_sites",
    "copy_helper_symbolized_sites",
    "dispose_helper_symbolized_sites",
    "captured_object_lifetime_sites",
    "throws_propagation_sites",
    "unwind_cleanup_sites",
    "bridged_error_sites",
    "nested_cleanup_sites",
    "foreign_boundary_sites",
    "async_continuation_sites",
    "continuation_resume_sites",
    "continuation_suspend_sites",
    "async_state_machine_sites",
    "cancellation_propagation_sites",
    "actor_isolation_sites",
    "actor_hop_sites",
    "sendability_check_sites",
    "reentrancy_policy_sites",
    "imported_actor_api_sites",
    "contract_violation_sites",
    "arc_semantics_landed",
    "block_escape_semantics_landed",
    "throws_cleanup_semantics_landed",
    "async_task_semantics_landed",
    "actor_semantics_landed",
    "foreign_boundary_semantics_landed",
    "deterministic",
    "ready_for_lowering_and_runtime",
    "replay_key",
]

POSITIVE_MIN_COUNTS = {
    "arc_ownership_qualified_sites": 2,
    "retain_insertion_sites": 1,
    "release_insertion_sites": 1,
    "weak_zeroing_sites": 1,
    "autoreleasepool_scope_sites": 2,
    "cleanup_order_exit_sites": 2,
    "block_literal_sites": 1,
    "stack_to_heap_promotion_sites": 1,
    "byref_forwarding_cell_sites": 1,
    "copy_helper_required_sites": 1,
    "dispose_helper_required_sites": 1,
    "captured_object_lifetime_sites": 3,
    "throws_propagation_sites": 2,
    "bridged_error_sites": 8,
    "foreign_boundary_sites": 7,
    "async_continuation_sites": 19,
    "cancellation_propagation_sites": 1,
    "reentrancy_policy_sites": 1,
    "imported_actor_api_sites": 1,
}

LANDED_FLAGS = [
    "arc_semantics_landed",
    "block_escape_semantics_landed",
    "throws_cleanup_semantics_landed",
    "async_task_semantics_landed",
    "actor_semantics_landed",
    "foreign_boundary_semantics_landed",
]

SOURCE_REPLAY_SEGMENTS = [
    "arc=",
    "weak=",
    "autoreleasepool=",
    "blocks=",
    "throws=",
    "async=",
    "actors=",
    "foreign=",
    "violations=",
]

RUNTIME_REPLAY_SEGMENTS = [
    *SOURCE_REPLAY_SEGMENTS[:-1],
    "violations=0",
]

CONTRACT_TOKENS = [
    "Objc3EffectsOwnershipSemanticModelSummary",
    "kObjc3EffectsOwnershipSemanticModelContractId",
    "kObjc3EffectsOwnershipSemanticModelSurfacePath",
]

SEMANTIC_PASS_TOKENS = [
    "BuildEffectsOwnershipSemanticModelSummary",
    "retain_release_operation_summary",
    "weak_unowned_semantics_summary",
    "autoreleasepool_scope_summary",
    "block_storage_escape_semantics_summary",
    "block_copy_dispose_semantics_summary",
    "throws_propagation_summary",
    "unwind_cleanup_summary",
    "ns_error_bridging_summary",
]

FRONTEND_ARTIFACT_TOKENS = [
    "BuildEffectsOwnershipSemanticModelSummaryJson",
    "objc_effects_ownership_semantic_model",
    "effects_ownership_semantic_model_summary",
]

LOWERING_CONTRACT_TOKENS = [
    "kObjc3RetainReleaseOperationLoweringLaneContract",
    "kObjc3AutoreleasePoolScopeLoweringLaneContract",
    "kObjc3WeakUnownedSemanticsLoweringLaneContract",
    "kObjc3BlockStorageEscapeLoweringLaneContract",
    "kObjc3BlockCopyDisposeLoweringLaneContract",
    "kObjc3ThrowsPropagationLoweringLaneContract",
    "kObjc3NSErrorBridgingLoweringLaneContract",
    "kObjc3UnwindCleanupLoweringLaneContract",
    "kObjc3AsyncContinuationLoweringLaneContract",
    "kObjc3ActorIsolationSendabilityLoweringLaneContract",
    "kObjc3ConcurrencyActorLoweringMetadataLaneContract",
    "kObjc3InteropForeignCallLifetimeLoweringDependencyContractId",
    "kObjc3RuntimeAllocateAsyncContinuationI32Symbol",
    "kObjc3RuntimeResumeAsyncContinuationI32Symbol",
]

IR_METADATA_TOKENS = [
    "lowering_retain_release_operation_replay_key",
    "lowering_autoreleasepool_scope_replay_key",
    "lowering_weak_unowned_semantics_replay_key",
    "lowering_block_storage_escape_replay_key",
    "lowering_block_copy_dispose_replay_key",
    "lowering_throws_propagation_replay_key",
    "lowering_ns_error_bridging_replay_key",
    "lowering_unwind_cleanup_replay_key",
    "lowering_async_continuation_replay_key",
    "lowering_actor_isolation_sendability_replay_key",
    "lowering_actor_lowering_metadata_replay_key",
    "lowering_interop_foreign_call_lifetime_replay_key",
]

VALIDATION_COMMANDS = [
    "python scripts/build_objc3c_effects_ownership_semantic_model.py --check",
    "python -m pytest tests/tooling/test_build_objc3c_effects_ownership_semantic_model.py",
    "npm run objc3c -- test-execution-replay",
    "npm run objc3c -- test-lowering-runtime-stress",
    "npm run objc3c -- test-full",
]
