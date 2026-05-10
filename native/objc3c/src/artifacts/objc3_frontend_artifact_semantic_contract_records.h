#pragma once

#include <cstddef>
#include <string>

inline constexpr const char *kObjc3ActorIsolationSendabilityLoweringLaneContract =
    "objc3c.actor.isolation.sendability.lowering.v1";
inline constexpr const char *kObjc3ConcurrencyActorLoweringMetadataLaneContract =
    "objc3c.actor.lowering.metadata.contract.v1";
inline constexpr const char *kObjc3ConcurrencyReplayRaceGuardLoweringLaneContract =
    "objc3c.concurrency.replay.race.guard.lowering.v1";

struct Objc3ActorIsolationSendabilityLoweringContract {
  std::size_t actor_isolation_sites = 0;
  std::size_t sendability_check_sites = 0;
  std::size_t cross_actor_hop_sites = 0;
  std::size_t non_sendable_capture_sites = 0;
  std::size_t sendable_transfer_sites = 0;
  std::size_t isolation_boundary_sites = 0;
  std::size_t guard_blocked_sites = 0;
  std::size_t contract_violation_sites = 0;
  bool deterministic = true;
};

struct Objc3ActorLoweringMetadataContract {
  std::size_t actor_interface_sites = 0;
  std::size_t actor_method_sites = 0;
  std::size_t actor_metadata_record_sites = 0;
  std::size_t nonisolated_entry_sites = 0;
  std::size_t executor_affinity_sites = 0;
  std::size_t actor_hop_artifact_sites = 0;
  std::size_t actor_isolation_thunk_sites = 0;
  std::size_t replay_proof_dependency_sites = 0;
  std::size_t race_guard_dependency_sites = 0;
  std::size_t task_handoff_sites = 0;
  std::size_t guard_blocked_sites = 0;
  std::size_t contract_violation_sites = 0;
  bool deterministic = true;
};

struct Objc3ConcurrencyReplayRaceGuardLoweringContract {
  std::size_t concurrency_replay_sites = 0;
  std::size_t replay_proof_sites = 0;
  std::size_t race_guard_sites = 0;
  std::size_t task_handoff_sites = 0;
  std::size_t actor_isolation_sites = 0;
  std::size_t deterministic_schedule_sites = 0;
  std::size_t guard_blocked_sites = 0;
  std::size_t contract_violation_sites = 0;
  bool deterministic = true;
};

inline constexpr const char *kObjc3AsyncContinuationLoweringLaneContract =
    "objc3c.async.continuation.lowering.v1";
inline constexpr const char
    *kObjc3AwaitLoweringSuspensionStateLoweringLaneContract =
        "objc3c.await.lowering.suspension.state.lowering.v1";

struct Objc3AsyncContinuationLoweringContract {
  std::size_t async_continuation_sites = 0;
  std::size_t async_keyword_sites = 0;
  std::size_t async_function_sites = 0;
  std::size_t continuation_allocation_sites = 0;
  std::size_t continuation_resume_sites = 0;
  std::size_t continuation_suspend_sites = 0;
  std::size_t async_state_machine_sites = 0;
  std::size_t normalized_sites = 0;
  std::size_t gate_blocked_sites = 0;
  std::size_t contract_violation_sites = 0;
  bool deterministic = true;
};

struct Objc3AwaitLoweringSuspensionStateLoweringContract {
  std::size_t await_suspension_sites = 0;
  std::size_t await_keyword_sites = 0;
  std::size_t await_suspension_point_sites = 0;
  std::size_t await_resume_sites = 0;
  std::size_t await_state_machine_sites = 0;
  std::size_t await_continuation_sites = 0;
  std::size_t normalized_sites = 0;
  std::size_t gate_blocked_sites = 0;
  std::size_t contract_violation_sites = 0;
  bool deterministic = true;
};

inline constexpr const char *kObjc3TaskRuntimeInteropCancellationLoweringLaneContract =
    "objc3c.task.runtime.interop.cancellation.lowering.v1";

struct Objc3TaskRuntimeInteropCancellationLoweringContract {
  std::size_t task_runtime_sites = 0;
  std::size_t task_runtime_interop_sites = 0;
  std::size_t cancellation_probe_sites = 0;
  std::size_t cancellation_handler_sites = 0;
  std::size_t runtime_resume_sites = 0;
  std::size_t runtime_cancel_sites = 0;
  std::size_t normalized_sites = 0;
  std::size_t guard_blocked_sites = 0;
  std::size_t contract_violation_sites = 0;
  bool deterministic = true;
};

inline constexpr const char *kObjc3ControlFlowControlFlowSafetyLoweringContractId =
    "objc3c.control_flow.control.flow.safety.lowering.v1";
inline constexpr const char *kObjc3ControlFlowControlFlowSafetyLoweringSurfacePath =
    "frontend.pipeline.semantic_surface."
    "objc_control_flow_control_flow_safety_lowering_contract";
inline constexpr const char *kObjc3ControlFlowControlFlowSafetyLoweringGuardModel =
    "native-lowering-executes-guard-clauses-via-short-circuit-control-flow-and-else-edge-cleanup";
inline constexpr const char *kObjc3ControlFlowControlFlowSafetyLoweringMatchModel =
    "native-lowering-executes-literal-default-wildcard-and-binding-match-arms-while-result-case-patterns-remain-explicitly-fail-closed";
inline constexpr const char *kObjc3ControlFlowControlFlowSafetyLoweringDeferModel =
    "native-lowering-registers-defer-cleanups-per-scope-and-emits-lifo-cleanup-insertion-on-scope-exit";
inline constexpr const char
    *kObjc3ControlFlowControlFlowSafetyLoweringAuthorityModel =
        "control_flow-source-closure-plus-control_flow-semantic-model-own-the-current-lowering-boundary";
inline constexpr const char
    *kObjc3ControlFlowControlFlowSafetyLoweringFailClosedModel =
        "native-ir-emission-fails-closed-with-o3l300-on-result-case-match-patterns-until-a-runtime-result-payload-abi-lands";

struct Objc3ControlFlowControlFlowSafetyLoweringContract {
  std::size_t guard_statement_sites = 0;
  std::size_t guard_clause_sites = 0;
  std::size_t match_statement_sites = 0;
  std::size_t defer_statement_sites = 0;
  std::size_t live_guard_short_circuit_sites = 0;
  std::size_t live_match_dispatch_sites = 0;
  std::size_t live_defer_cleanup_sites = 0;
  std::size_t fail_closed_guard_short_circuit_sites = 0;
  std::size_t fail_closed_match_dispatch_sites = 0;
  std::size_t fail_closed_defer_cleanup_sites = 0;
  std::size_t deterministic_fail_closed_sites = 0;
  std::size_t contract_violation_sites = 0;
  bool deterministic = true;
};

inline constexpr const char *kObjc3OwnershipQualifierLoweringLaneContract =
    "objc3c.ownership.qualifier.lowering.v1";
inline constexpr const char *kObjc3RetainReleaseOperationLoweringLaneContract =
    "objc3c.retain.release.operation.lowering.v1";
inline constexpr const char *kObjc3AutoreleasePoolScopeLoweringLaneContract =
    "objc3c.autoreleasepool.scope.lowering.v1";
inline constexpr const char *kObjc3WeakUnownedSemanticsLoweringLaneContract =
    "objc3c.weak.unowned.semantics.lowering.v1";
inline constexpr const char *kObjc3ArcDiagnosticsFixitLoweringLaneContract =
    "objc3c.arc.diagnostics.fixit.lowering.v1";

struct Objc3OwnershipQualifierLoweringContract {
  std::size_t ownership_qualifier_sites = 0;
  std::size_t invalid_ownership_qualifier_sites = 0;
  std::size_t object_pointer_type_annotation_sites = 0;
  bool deterministic = true;
};

struct Objc3RetainReleaseOperationLoweringContract {
  std::size_t ownership_qualified_sites = 0;
  std::size_t retain_insertion_sites = 0;
  std::size_t release_insertion_sites = 0;
  std::size_t autorelease_insertion_sites = 0;
  std::size_t contract_violation_sites = 0;
  bool deterministic = true;
};

struct Objc3AutoreleasePoolScopeLoweringContract {
  std::size_t scope_sites = 0;
  std::size_t scope_symbolized_sites = 0;
  unsigned max_scope_depth = 0;
  std::size_t scope_entry_transition_sites = 0;
  std::size_t scope_exit_transition_sites = 0;
  std::size_t contract_violation_sites = 0;
  bool deterministic = true;
};

struct Objc3WeakUnownedSemanticsLoweringContract {
  std::size_t ownership_candidate_sites = 0;
  std::size_t weak_reference_sites = 0;
  std::size_t unowned_reference_sites = 0;
  std::size_t unowned_safe_reference_sites = 0;
  std::size_t weak_unowned_conflict_sites = 0;
  std::size_t contract_violation_sites = 0;
  bool deterministic = true;
};

struct Objc3ArcDiagnosticsFixitLoweringContract {
  std::size_t ownership_arc_diagnostic_candidate_sites = 0;
  std::size_t ownership_arc_fixit_available_sites = 0;
  std::size_t ownership_arc_profiled_sites = 0;
  std::size_t ownership_arc_weak_unowned_conflict_diagnostic_sites = 0;
  std::size_t ownership_arc_empty_fixit_hint_sites = 0;
  std::size_t contract_violation_sites = 0;
  bool deterministic = true;
};

inline constexpr const char *kObjc3OwnershipSystemExtensionLoweringContractId =
    "objc3c.ownership.system.extension.lowering.contract.v1";
inline constexpr const char *kObjc3OwnershipSystemExtensionLoweringSurfacePath =
    "frontend.pipeline.semantic_surface."
    "objc_ownership_system_extension_lowering_contract";
inline constexpr const char *kObjc3OwnershipSystemExtensionLoweringModel =
    "cleanup-resource-borrowed-and-retainable-family-sema-packets-now-feed-one-deterministic-ownership-lowering-contract-for-manifest-and-ir-carriage";
inline constexpr const char *kObjc3OwnershipSystemExtensionLoweringDeferredModel =
    "live-cleanup-runtime-carriers-borrowed-lifetime-enforcement-and-runnable-retainable-family-runtime-interop-remain-later-system-extension-runtime-work";
inline constexpr const char *kObjc3OwnershipSystemExtensionLoweringLaneContract =
    "objc3c.ownership.system.extension.lowering.contract.v1";
inline constexpr const char *kObjc3OwnershipBorrowedRetainableAbiCompletionContractId =
    "objc3c.ownership.borrowed.retainable.family.abi.completion.v1";
inline constexpr const char *kObjc3OwnershipBorrowedRetainableAbiCompletionSurfacePath =
    "frontend.pipeline.semantic_surface."
    "objc_ownership_borrowed_pointer_and_retainable_family_abi_completion";
inline constexpr const char *kObjc3OwnershipBorrowedRetainableAbiCompletionLaneContract =
    "objc3c.ownership.borrowed.retainable.family.abi.completion.v1";

struct Objc3OwnershipSystemExtensionLoweringContract {
  std::size_t cleanup_hook_sites = 0;
  std::size_t resource_local_sites = 0;
  std::size_t cleanup_owned_local_sites = 0;
  std::size_t resource_move_capture_sites = 0;
  std::size_t borrowed_parameter_sites = 0;
  std::size_t borrowed_return_callable_sites = 0;
  std::size_t borrowed_escape_candidate_sites = 0;
  std::size_t explicit_capture_item_sites = 0;
  std::size_t retainable_family_callable_sites = 0;
  std::size_t retainable_family_operation_callable_sites = 0;
  std::size_t retainable_family_alias_callable_sites = 0;
  std::size_t guard_blocked_sites = 0;
  std::size_t contract_violation_sites = 0;
  bool deterministic = true;
};

inline constexpr const char *kObjc3BlockLiteralCaptureLoweringLaneContract =
    "objc3c.block.literal.capture.lowering.v1";
inline constexpr const char *kObjc3BlockSourceModelCompletionLaneContract =
    "objc3c.block.source.model.v1";
inline constexpr const char *kObjc3BlockSourceStorageAnnotationLaneContract =
    "objc3c.block.source.storage.annotations.v1";

struct Objc3BlockLiteralCaptureLoweringContract {
  std::size_t block_literal_sites = 0;
  std::size_t block_parameter_entries = 0;
  std::size_t block_capture_entries = 0;
  std::size_t block_body_statement_entries = 0;
  std::size_t block_empty_capture_sites = 0;
  std::size_t block_nondeterministic_capture_sites = 0;
  std::size_t block_non_normalized_sites = 0;
  std::size_t contract_violation_sites = 0;
  bool deterministic = true;
};

struct Objc3BlockSourceModelCompletionContract {
  std::size_t block_literal_sites = 0;
  std::size_t signature_entries_total = 0;
  std::size_t explicit_typed_parameter_entries_total = 0;
  std::size_t implicit_parameter_entries_total = 0;
  std::size_t capture_inventory_entries_total = 0;
  std::size_t byvalue_readonly_capture_entries_total = 0;
  std::size_t invoke_surface_entries_total = 0;
  std::size_t non_normalized_sites = 0;
  std::size_t contract_violation_sites = 0;
  bool deterministic = true;
};

struct Objc3BlockSourceStorageAnnotationContract {
  std::size_t block_literal_sites = 0;
  std::size_t capture_entries_total = 0;
  std::size_t mutated_capture_entries_total = 0;
  std::size_t byref_capture_entries_total = 0;
  std::size_t copy_helper_intent_sites = 0;
  std::size_t dispose_helper_intent_sites = 0;
  std::size_t heap_candidate_sites = 0;
  std::size_t expression_sites = 0;
  std::size_t global_initializer_sites = 0;
  std::size_t binding_initializer_sites = 0;
  std::size_t assignment_value_sites = 0;
  std::size_t return_value_sites = 0;
  std::size_t call_argument_sites = 0;
  std::size_t message_argument_sites = 0;
  std::size_t non_normalized_sites = 0;
  std::size_t contract_violation_sites = 0;
  bool deterministic = true;
};

struct Objc3BlockAbiInvokeTrampolineLoweringContract {
  std::size_t block_literal_sites = 0;
  std::size_t invoke_argument_slots_total = 0;
  std::size_t capture_word_count_total = 0;
  std::size_t parameter_entries_total = 0;
  std::size_t capture_entries_total = 0;
  std::size_t body_statement_entries_total = 0;
  std::size_t descriptor_symbolized_sites = 0;
  std::size_t invoke_trampoline_symbolized_sites = 0;
  std::size_t missing_invoke_trampoline_sites = 0;
  std::size_t non_normalized_layout_sites = 0;
  std::size_t contract_violation_sites = 0;
  bool deterministic = true;
};

struct Objc3BlockStorageEscapeLoweringContract {
  std::size_t block_literal_sites = 0;
  std::size_t mutable_capture_count_total = 0;
  std::size_t byref_slot_count_total = 0;
  std::size_t parameter_entries_total = 0;
  std::size_t capture_entries_total = 0;
  std::size_t body_statement_entries_total = 0;
  std::size_t requires_byref_cells_sites = 0;
  std::size_t escape_analysis_enabled_sites = 0;
  std::size_t escape_to_heap_sites = 0;
  std::size_t escape_profile_normalized_sites = 0;
  std::size_t byref_layout_symbolized_sites = 0;
  std::size_t contract_violation_sites = 0;
  bool deterministic = true;
};

struct Objc3BlockCopyDisposeLoweringContract {
  std::size_t block_literal_sites = 0;
  std::size_t mutable_capture_count_total = 0;
  std::size_t byref_slot_count_total = 0;
  std::size_t parameter_entries_total = 0;
  std::size_t capture_entries_total = 0;
  std::size_t body_statement_entries_total = 0;
  std::size_t copy_helper_required_sites = 0;
  std::size_t dispose_helper_required_sites = 0;
  std::size_t profile_normalized_sites = 0;
  std::size_t copy_helper_symbolized_sites = 0;
  std::size_t dispose_helper_symbolized_sites = 0;
  std::size_t contract_violation_sites = 0;
  bool deterministic = true;
};

struct Objc3BlockDeterminismPerfBaselineLoweringContract {
  std::size_t block_literal_sites = 0;
  std::size_t baseline_weight_total = 0;
  std::size_t parameter_entries_total = 0;
  std::size_t capture_entries_total = 0;
  std::size_t body_statement_entries_total = 0;
  std::size_t deterministic_capture_sites = 0;
  std::size_t heavy_tier_sites = 0;
  std::size_t normalized_profile_sites = 0;
  std::size_t contract_violation_sites = 0;
  bool deterministic = true;
};

inline constexpr const char *kObjc3ThrowsPropagationLoweringLaneContract =
    "objc3c.throws.propagation.lowering.v1";
inline constexpr const char *kObjc3UnwindCleanupLoweringLaneContract =
    "objc3c.unwind.cleanup.lowering.v1";

struct Objc3ThrowsPropagationLoweringContract {
  std::size_t throws_propagation_sites = 0;
  std::size_t namespace_segment_sites = 0;
  std::size_t import_edge_candidate_sites = 0;
  std::size_t object_pointer_type_sites = 0;
  std::size_t pointer_declarator_sites = 0;
  std::size_t normalized_sites = 0;
  std::size_t cache_invalidation_candidate_sites = 0;
  std::size_t contract_violation_sites = 0;
  bool deterministic = true;
};

struct Objc3UnwindCleanupLoweringContract {
  std::size_t unwind_cleanup_sites = 0;
  std::size_t unwind_edge_sites = 0;
  std::size_t cleanup_scope_sites = 0;
  std::size_t cleanup_emit_sites = 0;
  std::size_t landing_pad_sites = 0;
  std::size_t cleanup_resume_sites = 0;
  std::size_t normalized_sites = 0;
  std::size_t guard_blocked_sites = 0;
  std::size_t contract_violation_sites = 0;
  bool deterministic = true;
};

inline constexpr const char *kObjc3ResultLikeLoweringLaneContract =
    "objc3c.result.like.lowering.v1";
inline constexpr const char *kObjc3NSErrorBridgingLoweringLaneContract =
    "objc3c.ns.error.bridging.lowering.v1";

struct Objc3ResultLikeLoweringContract {
  std::size_t result_like_sites = 0;
  std::size_t result_success_sites = 0;
  std::size_t result_failure_sites = 0;
  std::size_t result_branch_sites = 0;
  std::size_t result_payload_sites = 0;
  std::size_t normalized_sites = 0;
  std::size_t branch_merge_sites = 0;
  std::size_t contract_violation_sites = 0;
  bool deterministic = true;
};

struct Objc3NSErrorBridgingLoweringContract {
  std::size_t ns_error_bridging_sites = 0;
  std::size_t ns_error_parameter_sites = 0;
  std::size_t ns_error_out_parameter_sites = 0;
  std::size_t ns_error_bridge_path_sites = 0;
  std::size_t failable_call_sites = 0;
  std::size_t normalized_sites = 0;
  std::size_t bridge_boundary_sites = 0;
  std::size_t contract_violation_sites = 0;
  bool deterministic = true;
};

inline constexpr const char *kObjc3ModuleImportGraphLoweringLaneContract =
    "objc3c.module.import.graph.lowering.v1";
inline constexpr const char *kObjc3NamespaceCollisionShadowingLoweringLaneContract =
    "objc3c.namespace.collision.shadowing.lowering.v1";
inline constexpr const char *kObjc3PublicPrivateApiPartitionLoweringLaneContract =
    "objc3c.public.private.api.partition.lowering.v1";
inline constexpr const char *kObjc3IncrementalModuleCacheInvalidationLoweringLaneContract =
    "objc3c.incremental.module.cache.invalidation.lowering.v1";
inline constexpr const char *kObjc3CrossModuleConformanceLoweringLaneContract =
    "objc3c.cross.module.conformance.lowering.v1";

struct Objc3ModuleImportGraphLoweringContract {
  std::size_t module_import_graph_sites = 0;
  std::size_t import_edge_candidate_sites = 0;
  std::size_t namespace_segment_sites = 0;
  std::size_t object_pointer_type_sites = 0;
  std::size_t pointer_declarator_sites = 0;
  std::size_t normalized_sites = 0;
  std::size_t contract_violation_sites = 0;
  bool deterministic = true;
};

struct Objc3NamespaceCollisionShadowingLoweringContract {
  std::size_t namespace_collision_shadowing_sites = 0;
  std::size_t namespace_segment_sites = 0;
  std::size_t import_edge_candidate_sites = 0;
  std::size_t object_pointer_type_sites = 0;
  std::size_t pointer_declarator_sites = 0;
  std::size_t normalized_sites = 0;
  std::size_t contract_violation_sites = 0;
  bool deterministic = true;
};

struct Objc3PublicPrivateApiPartitionLoweringContract {
  std::size_t public_private_api_partition_sites = 0;
  std::size_t namespace_segment_sites = 0;
  std::size_t import_edge_candidate_sites = 0;
  std::size_t object_pointer_type_sites = 0;
  std::size_t pointer_declarator_sites = 0;
  std::size_t normalized_sites = 0;
  std::size_t contract_violation_sites = 0;
  bool deterministic = true;
};

struct Objc3IncrementalModuleCacheInvalidationLoweringContract {
  std::size_t incremental_module_cache_invalidation_sites = 0;
  std::size_t namespace_segment_sites = 0;
  std::size_t import_edge_candidate_sites = 0;
  std::size_t object_pointer_type_sites = 0;
  std::size_t pointer_declarator_sites = 0;
  std::size_t normalized_sites = 0;
  std::size_t cache_invalidation_candidate_sites = 0;
  std::size_t contract_violation_sites = 0;
  bool deterministic = true;
};

struct Objc3CrossModuleConformanceLoweringContract {
  std::size_t cross_module_conformance_sites = 0;
  std::size_t namespace_segment_sites = 0;
  std::size_t import_edge_candidate_sites = 0;
  std::size_t object_pointer_type_sites = 0;
  std::size_t pointer_declarator_sites = 0;
  std::size_t normalized_sites = 0;
  std::size_t cache_invalidation_candidate_sites = 0;
  std::size_t contract_violation_sites = 0;
  bool deterministic = true;
};

inline constexpr const char *kObjc3InteropInteropLoweringContractId =
    "objc3c.interop.interop.lowering.and.abi.contract.v1";
inline constexpr const char *kObjc3InteropInteropLoweringSurfacePath =
    "frontend.pipeline.semantic_surface."
    "objc_interop_interop_lowering_and_abi_contract";
inline constexpr const char *kObjc3InteropInteropLoweringModel =
    "interop-foreign-callable-sema-runtime-parity-cpp-interaction-swift-isolation-and-interface-preservation-packets-now-feed-one-deterministic-lowering-contract-for-manifest-and-ir-carriage";
inline constexpr const char *kObjc3InteropInteropLoweringDeferredModel =
    "live-ffi-call-lowering-ownership-bridge-helper-emission-error-runtime-integration-and-cross-module-runtime-consumption-remain-later-interop-runtime-work";
inline constexpr const char *kObjc3InteropInteropLoweringLaneContract =
    "objc3c.interop.interop.lowering.abi.contract.v1";
inline constexpr const char *kObjc3InteropForeignCallLifetimeLoweringContractId =
    "objc3c.interop.foreign.call.and.lifetime.lowering.v1";
inline constexpr const char *kObjc3InteropForeignCallLifetimeLoweringSurfacePath =
    "frontend.pipeline.semantic_surface."
    "objc_interop_foreign_call_and_lifetime_lowering";
inline constexpr const char *kObjc3InteropForeignCallLifetimeLoweringModel =
    "foreign-calls-and-cpp-swift-facing-free-functions-now-lower-through-one-deterministic-interop-call-boundary-that-preserves-ownership-lifetime-and-annotation-facts-in-manifest-and-ir";
inline constexpr const char
    *kObjc3InteropForeignCallLifetimeLoweringDeferredModel =
        "cross-module-runtime-consumption-live-foreign-linking-and-runnable-host-language-integration-remain-later-interop-closeout-work";

struct Objc3InteropInteropLoweringContract {
  std::size_t foreign_callable_sites = 0;
  std::size_t c_foreign_callable_sites = 0;
  std::size_t objc_runtime_parity_callable_sites = 0;
  std::size_t ownership_bridge_callable_sites = 0;
  std::size_t error_surface_sites = 0;
  std::size_t async_boundary_sites = 0;
  std::size_t swift_concurrency_metadata_sites = 0;
  std::size_t interface_preserved_foreign_callable_sites = 0;
  std::size_t interface_preserved_metadata_annotation_sites = 0;
  std::size_t guard_blocked_sites = 0;
  std::size_t contract_violation_sites = 0;
  bool deterministic = true;
};

struct Objc3InteropForeignCallLifetimeLoweringContract {
  std::size_t foreign_callable_sites = 0;
  std::size_t c_foreign_callable_sites = 0;
  std::size_t objc_runtime_parity_callable_sites = 0;
  std::size_t ownership_bridge_sites = 0;
  std::size_t lifetime_bridge_sites = 0;
  std::size_t metadata_preservation_sites = 0;
  std::size_t guard_blocked_sites = 0;
  std::size_t contract_violation_sites = 0;
  bool deterministic = true;
};

inline constexpr const char
    *kObjc3InteropFfiMetadataInterfacePreservationContractId =
        "objc3c.interop.ffi.metadata.interface.preservation.v1";
inline constexpr const char
    *kObjc3InteropFfiMetadataInterfacePreservationSurfacePath =
        "frontend.pipeline.semantic_surface."
        "objc_interop_ffi_metadata_and_interface_preservation";
inline constexpr const char
    *kObjc3InteropFfiMetadataInterfacePreservationImportArtifactMemberName =
        "objc_interop_ffi_metadata_and_interface_preservation";
inline constexpr const char
    *kObjc3InteropFfiMetadataInterfacePreservationSourceContractId =
        kObjc3InteropForeignCallLifetimeLoweringContractId;
inline constexpr const char
    *kObjc3InteropFfiMetadataInterfacePreservationPreservationModel =
        "provider-runtime-import-surfaces-now-preserve-interop-callable-and-annotation-metadata-beyond-local-manifest-ir-and-object-emission";
inline constexpr const char
    *kObjc3InteropFfiMetadataInterfacePreservationSourceModel =
        "local-lane-c-interop-lowering-and-lane-a-preservation-packets-feed-one-runtime-import-surface-summary-for-separate-compilation-replay";
inline constexpr const char
    *kObjc3InteropFfiMetadataInterfacePreservationFailClosedModel =
        "missing-import-surface-packet-drifted-replay-keys-or-non-deterministic-provider-preservation-disables-cross-module-interop-preservation-claims";

struct Objc3InteropFfiMetadataInterfacePreservationContract {
  std::size_t local_foreign_callable_count = 0;
  std::size_t local_metadata_preservation_sites = 0;
  std::size_t local_interface_annotation_sites = 0;
  std::size_t imported_module_count = 0;
  std::size_t imported_foreign_callable_count = 0;
  std::size_t imported_metadata_preservation_sites = 0;
  std::size_t imported_interface_annotation_sites = 0;
  bool runtime_import_artifact_ready = false;
  bool separate_compilation_preservation_ready = false;
  bool deterministic = false;
};

inline constexpr const char *kObjc3MetaprogrammingExpansionLoweringContractId =
    "objc3c.metaprogramming.expansion.lowering.contract.v1";
inline constexpr const char *kObjc3MetaprogrammingExpansionLoweringSurfacePath =
    "frontend.pipeline.semantic_surface."
    "objc_metaprogramming_expansion_and_lowering_contract";
inline constexpr const char *kObjc3MetaprogrammingExpansionLoweringModel =
    "metaprogramming-derived-selector-inventory-macro-replay-visibility-and-synthesized-property-metadata-now-feed-one-deterministic-lowering-contract-for-manifest-and-ir-carriage";
inline constexpr const char *kObjc3MetaprogrammingExpansionLoweringDeferredModel =
    "runnable-derive-body-emission-macro-execution-and-property-behavior-runtime-materialization-remain-later-expansion-runtime-work";
inline constexpr const char *kObjc3MetaprogrammingExpansionLoweringLaneContract =
    "objc3c.metaprogramming.expansion.lowering.contract.v1";
inline constexpr const char *kObjc3MetaprogrammingSynthesizedArtifactEmissionContractId =
    "objc3c.metaprogramming.synthesized.ast.ir.emission.v1";
inline constexpr const char *kObjc3MetaprogrammingSynthesizedArtifactEmissionSurfacePath =
    "frontend.pipeline.semantic_surface."
    "objc_metaprogramming_synthesized_ast_and_ir_emission";
inline constexpr const char *kObjc3MetaprogrammingSynthesizedArtifactEmissionModel =
    "supported-derive-macro-and-property-behavior-sites-now-materialize-deterministic-synthesized-ir-artifacts-and-runtime-visible-method-bodies";
inline constexpr const char
    *kObjc3MetaprogrammingSynthesizedArtifactEmissionDeferredModel =
        "cross-module-preservation-expansion-host-execution-and-cached-macro-toolchain-integration-remain-deferred-to-later-runtime-work";
inline constexpr const char *kObjc3MetaprogrammingSynthesizedArtifactEmissionLaneContract =
    "objc3c.metaprogramming.synthesized.ast.ir.emission.v1";

struct Objc3MetaprogrammingExpansionLoweringContract {
  std::size_t derive_inventory_sites = 0;
  std::size_t derived_selector_artifact_sites = 0;
  std::size_t macro_replay_visible_sites = 0;
  std::size_t property_behavior_sites = 0;
  std::size_t synthesized_binding_sites = 0;
  std::size_t synthesized_getter_sites = 0;
  std::size_t synthesized_setter_sites = 0;
  std::size_t replay_visible_metadata_sites = 0;
  std::size_t guard_blocked_sites = 0;
  std::size_t contract_violation_sites = 0;
  bool deterministic = true;
};

struct Objc3MetaprogrammingSynthesizedArtifactEmissionContract {
  std::size_t derive_inventory_sites = 0;
  std::size_t emitted_derive_method_sites = 0;
  std::size_t emitted_macro_artifact_sites = 0;
  std::size_t emitted_property_behavior_artifact_sites = 0;
  std::size_t emitted_global_artifact_sites = 0;
  std::size_t emitted_runtime_method_list_sites = 0;
  std::size_t guard_blocked_sites = 0;
  std::size_t contract_violation_sites = 0;
  bool deterministic = true;
};

struct Objc3IRMetaprogrammingDerivedMethodBundle {
  std::string implementation_name;
  std::string declaration_owner_identity;
  std::string export_owner_identity;
  std::string derive_name;
  std::string selector;
  std::string emitted_symbol;
  std::size_t parameter_count = 0;
  unsigned line = 1;
  unsigned column = 1;
};

struct Objc3IRMetaprogrammingMacroArtifactBundle {
  std::string function_name;
  std::string macro_name;
  std::string package_name;
  std::string provenance_name;
  std::string cache_key_name;
  std::string sandbox_policy_name;
  std::string emitted_symbol;
  unsigned line = 1;
  unsigned column = 1;
};

struct Objc3IRMetaprogrammingPropertyBehaviorArtifactBundle {
  std::string owner_kind;
  std::string owner_name;
  std::string declaration_owner_identity;
  std::string export_owner_identity;
  std::string property_name;
  std::string behavior_name;
  std::string binding_symbol;
  std::string emitted_symbol;
  unsigned line = 1;
  unsigned column = 1;
};

bool IsValidObjc3ActorIsolationSendabilityLoweringContract(
    const Objc3ActorIsolationSendabilityLoweringContract &contract);
std::string Objc3ActorIsolationSendabilityLoweringReplayKey(
    const Objc3ActorIsolationSendabilityLoweringContract &contract);
bool IsValidObjc3ActorLoweringMetadataContract(
    const Objc3ActorLoweringMetadataContract &contract);
std::string Objc3ActorLoweringMetadataReplayKey(
    const Objc3ActorLoweringMetadataContract &contract);
bool IsValidObjc3ConcurrencyReplayRaceGuardLoweringContract(
    const Objc3ConcurrencyReplayRaceGuardLoweringContract &contract);
std::string Objc3ConcurrencyReplayRaceGuardLoweringReplayKey(
    const Objc3ConcurrencyReplayRaceGuardLoweringContract &contract);
bool IsValidObjc3AsyncContinuationLoweringContract(
    const Objc3AsyncContinuationLoweringContract &contract);
std::string Objc3AsyncContinuationLoweringReplayKey(
    const Objc3AsyncContinuationLoweringContract &contract);
bool IsValidObjc3AwaitLoweringSuspensionStateLoweringContract(
    const Objc3AwaitLoweringSuspensionStateLoweringContract &contract);
std::string Objc3AwaitLoweringSuspensionStateLoweringReplayKey(
    const Objc3AwaitLoweringSuspensionStateLoweringContract &contract);
bool IsValidObjc3TaskRuntimeInteropCancellationLoweringContract(
    const Objc3TaskRuntimeInteropCancellationLoweringContract &contract);
std::string Objc3TaskRuntimeInteropCancellationLoweringReplayKey(
    const Objc3TaskRuntimeInteropCancellationLoweringContract &contract);
bool IsValidObjc3ControlFlowControlFlowSafetyLoweringContract(
    const Objc3ControlFlowControlFlowSafetyLoweringContract &contract);
std::string Objc3ControlFlowControlFlowSafetyLoweringReplayKey(
    const Objc3ControlFlowControlFlowSafetyLoweringContract &contract);
bool IsValidObjc3OwnershipQualifierLoweringContract(
    const Objc3OwnershipQualifierLoweringContract &contract);
std::string Objc3OwnershipQualifierLoweringReplayKey(
    const Objc3OwnershipQualifierLoweringContract &contract);
bool IsValidObjc3RetainReleaseOperationLoweringContract(
    const Objc3RetainReleaseOperationLoweringContract &contract);
std::string Objc3RetainReleaseOperationLoweringReplayKey(
    const Objc3RetainReleaseOperationLoweringContract &contract);
bool IsValidObjc3AutoreleasePoolScopeLoweringContract(
    const Objc3AutoreleasePoolScopeLoweringContract &contract);
std::string Objc3AutoreleasePoolScopeLoweringReplayKey(
    const Objc3AutoreleasePoolScopeLoweringContract &contract);
bool IsValidObjc3WeakUnownedSemanticsLoweringContract(
    const Objc3WeakUnownedSemanticsLoweringContract &contract);
std::string Objc3WeakUnownedSemanticsLoweringReplayKey(
    const Objc3WeakUnownedSemanticsLoweringContract &contract);
bool IsValidObjc3ArcDiagnosticsFixitLoweringContract(
    const Objc3ArcDiagnosticsFixitLoweringContract &contract);
std::string Objc3ArcDiagnosticsFixitLoweringReplayKey(
    const Objc3ArcDiagnosticsFixitLoweringContract &contract);
bool IsValidObjc3OwnershipSystemExtensionLoweringContract(
    const Objc3OwnershipSystemExtensionLoweringContract &contract);
std::string Objc3OwnershipSystemExtensionLoweringReplayKey(
    const Objc3OwnershipSystemExtensionLoweringContract &contract);
bool IsValidObjc3BlockSourceModelCompletionContract(
    const Objc3BlockSourceModelCompletionContract &contract);
std::string Objc3BlockSourceModelCompletionReplayKey(
    const Objc3BlockSourceModelCompletionContract &contract);
bool IsValidObjc3BlockSourceStorageAnnotationContract(
    const Objc3BlockSourceStorageAnnotationContract &contract);
std::string Objc3BlockSourceStorageAnnotationReplayKey(
    const Objc3BlockSourceStorageAnnotationContract &contract);
bool IsValidObjc3BlockLiteralCaptureLoweringContract(
    const Objc3BlockLiteralCaptureLoweringContract &contract);
std::string Objc3BlockLiteralCaptureLoweringReplayKey(
    const Objc3BlockLiteralCaptureLoweringContract &contract);
bool IsValidObjc3ThrowsPropagationLoweringContract(
    const Objc3ThrowsPropagationLoweringContract &contract);
std::string Objc3ThrowsPropagationLoweringReplayKey(
    const Objc3ThrowsPropagationLoweringContract &contract);
bool IsValidObjc3UnwindCleanupLoweringContract(
    const Objc3UnwindCleanupLoweringContract &contract);
std::string Objc3UnwindCleanupLoweringReplayKey(
    const Objc3UnwindCleanupLoweringContract &contract);
bool IsValidObjc3ResultLikeLoweringContract(
    const Objc3ResultLikeLoweringContract &contract);
std::string Objc3ResultLikeLoweringReplayKey(
    const Objc3ResultLikeLoweringContract &contract);
bool IsValidObjc3NSErrorBridgingLoweringContract(
    const Objc3NSErrorBridgingLoweringContract &contract);
std::string Objc3NSErrorBridgingLoweringReplayKey(
    const Objc3NSErrorBridgingLoweringContract &contract);
bool IsValidObjc3ModuleImportGraphLoweringContract(
    const Objc3ModuleImportGraphLoweringContract &contract);
std::string Objc3ModuleImportGraphLoweringReplayKey(
    const Objc3ModuleImportGraphLoweringContract &contract);
bool IsValidObjc3NamespaceCollisionShadowingLoweringContract(
    const Objc3NamespaceCollisionShadowingLoweringContract &contract);
std::string Objc3NamespaceCollisionShadowingLoweringReplayKey(
    const Objc3NamespaceCollisionShadowingLoweringContract &contract);
bool IsValidObjc3PublicPrivateApiPartitionLoweringContract(
    const Objc3PublicPrivateApiPartitionLoweringContract &contract);
std::string Objc3PublicPrivateApiPartitionLoweringReplayKey(
    const Objc3PublicPrivateApiPartitionLoweringContract &contract);
bool IsValidObjc3IncrementalModuleCacheInvalidationLoweringContract(
    const Objc3IncrementalModuleCacheInvalidationLoweringContract &contract);
std::string Objc3IncrementalModuleCacheInvalidationLoweringReplayKey(
    const Objc3IncrementalModuleCacheInvalidationLoweringContract &contract);
bool IsValidObjc3CrossModuleConformanceLoweringContract(
    const Objc3CrossModuleConformanceLoweringContract &contract);
std::string Objc3CrossModuleConformanceLoweringReplayKey(
    const Objc3CrossModuleConformanceLoweringContract &contract);
bool IsValidObjc3InteropInteropLoweringContract(
    const Objc3InteropInteropLoweringContract &contract);
std::string Objc3InteropInteropLoweringReplayKey(
    const Objc3InteropInteropLoweringContract &contract);
bool IsValidObjc3InteropForeignCallLifetimeLoweringContract(
    const Objc3InteropForeignCallLifetimeLoweringContract &contract);
std::string Objc3InteropForeignCallLifetimeLoweringReplayKey(
    const Objc3InteropForeignCallLifetimeLoweringContract &contract);
bool IsValidObjc3InteropFfiMetadataInterfacePreservationContract(
    const Objc3InteropFfiMetadataInterfacePreservationContract &contract);
std::string Objc3InteropFfiMetadataInterfacePreservationReplayKey(
    const Objc3InteropFfiMetadataInterfacePreservationContract &contract);
bool IsValidObjc3MetaprogrammingExpansionLoweringContract(
    const Objc3MetaprogrammingExpansionLoweringContract &contract);
std::string Objc3MetaprogrammingExpansionLoweringReplayKey(
    const Objc3MetaprogrammingExpansionLoweringContract &contract);
bool IsValidObjc3MetaprogrammingSynthesizedArtifactEmissionContract(
    const Objc3MetaprogrammingSynthesizedArtifactEmissionContract &contract);
std::string Objc3MetaprogrammingSynthesizedArtifactEmissionReplayKey(
    const Objc3MetaprogrammingSynthesizedArtifactEmissionContract &contract);
std::string Objc3MetaprogrammingModuleInterfaceReplayPreservationSummary();
std::string Objc3MetaprogrammingMacroHostProcessCacheRuntimeIntegrationSummary();
