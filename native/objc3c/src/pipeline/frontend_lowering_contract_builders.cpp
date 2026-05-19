#include <algorithm>
#include <cstddef>

#include "lower/contracts/concurrency_actor_contracts.h"
#include "lower/contracts/concurrency_task_lowering_contracts.h"
#include "lower/contracts/dispatch_control_lowering_contracts.h"
#include "lower/contracts/ownership_system_extension_contracts.h"
#include "pipeline/objc3_frontend_types.h"

namespace objc3::artifacts::frontend {

Objc3ActorIsolationSendabilityLoweringContract
BuildConcurrencyActorIsolationSendabilityLoweringContract(
    const Objc3ConcurrencyExecutorHopAffinityCompatibilitySummary &summary) {
  Objc3ActorIsolationSendabilityLoweringContract contract;
  contract.actor_isolation_sites =
      summary.executor_affinity_sites +
      summary.illegal_missing_executor_affinity_sites +
      summary.illegal_main_executor_detached_sites;
  contract.sendability_check_sites = summary.executor_affinity_sites;
  contract.cross_actor_hop_sites = summary.detached_task_creation_sites;
  contract.non_sendable_capture_sites = 0;
  contract.sendable_transfer_sites = summary.detached_task_creation_sites;
  contract.isolation_boundary_sites = summary.executor_affinity_sites;
  contract.guard_blocked_sites =
      summary.illegal_missing_executor_affinity_sites +
      summary.illegal_main_executor_detached_sites;
  contract.contract_violation_sites = 0;
  contract.deterministic =
      summary.deterministic && summary.ready_for_lowering_and_runtime &&
      contract.actor_isolation_sites ==
          contract.isolation_boundary_sites + contract.guard_blocked_sites;
  return contract;
}

Objc3OwnershipSystemExtensionLoweringContract
BuildOwnershipSystemExtensionLoweringContract(
    const Objc3OwnershipSystemExtensionSemanticModelSummary &semantic_summary,
    const Objc3OwnershipResourceMoveUseAfterMoveSemanticsSummary
        &resource_summary,
    const Objc3OwnershipBorrowedPointerEscapeAnalysisSummary &borrowed_summary,
    const Objc3OwnershipCaptureListRetainableFamilyLegalityCompletionSummary
        &family_summary) {
  // lowering-implementation anchor: the live cleanup/resource
  // helper-emission path continues to consume the single C001 Part 8 lowering
  // contract; this issue does not mint a second manifest surface.
  Objc3OwnershipSystemExtensionLoweringContract contract;
  contract.cleanup_hook_sites = semantic_summary.cleanup_attribute_sites +
                                semantic_summary.cleanup_sugar_sites;
  contract.resource_local_sites = semantic_summary.resource_attribute_sites +
                                  semantic_summary.resource_sugar_sites;
  contract.cleanup_owned_local_sites = resource_summary.cleanup_owned_local_sites;
  contract.resource_move_capture_sites =
      resource_summary.resource_move_capture_sites;
  contract.borrowed_parameter_sites = borrowed_summary.borrowed_parameter_sites;
  contract.borrowed_return_callable_sites =
      borrowed_summary.borrowed_return_callable_sites;
  contract.borrowed_escape_candidate_sites =
      borrowed_summary.borrowed_escape_candidate_sites;
  contract.explicit_capture_item_sites =
      family_summary.explicit_capture_item_sites;
  contract.retainable_family_callable_sites =
      family_summary.retainable_family_callable_sites;
  contract.retainable_family_operation_callable_sites =
      family_summary.retainable_family_operation_callable_sites;
  contract.retainable_family_alias_callable_sites =
      family_summary.retainable_family_alias_callable_sites;
  contract.guard_blocked_sites =
      resource_summary.illegal_non_resource_move_sites +
      resource_summary.illegal_use_after_move_sites +
      resource_summary.illegal_duplicate_move_sites +
      borrowed_summary.illegal_unproven_call_escape_sites +
      borrowed_summary.illegal_escaping_block_capture_sites +
      borrowed_summary.illegal_borrowed_return_sites +
      family_summary.illegal_duplicate_explicit_capture_sites +
      family_summary.illegal_non_object_capture_mode_sites +
      family_summary.illegal_unused_explicit_capture_sites +
      family_summary.illegal_conflicting_retainable_family_sites +
      family_summary.illegal_invalid_family_operation_shape_sites +
      family_summary.illegal_invalid_family_alias_shape_sites;
  contract.contract_violation_sites = 0;
  contract.deterministic =
      semantic_summary.deterministic &&
      semantic_summary.ready_for_lowering_and_runtime &&
      resource_summary.deterministic &&
      resource_summary.ready_for_lowering_and_runtime &&
      borrowed_summary.deterministic &&
      borrowed_summary.ready_for_lowering_and_runtime &&
      family_summary.deterministic &&
      family_summary.ready_for_lowering_and_runtime;
  return contract;
}

Objc3DispatchDispatchControlLoweringContract
BuildDispatchDispatchControlLoweringContract(
    const Objc3DispatchDispatchIntentSemanticModelSummary &semantic_summary,
    const Objc3DispatchDispatchIntentLegalitySummary &legality_summary,
    const Objc3DispatchDispatchIntentCompatibilitySummary
        &compatibility_summary) {
  Objc3DispatchDispatchControlLoweringContract contract;
  const std::size_t dispatch_intent_callable_capacity =
      compatibility_summary.callable_dispatch_intent_sites;
  const std::size_t dispatch_intent_container_capacity =
      compatibility_summary.container_dispatch_intent_sites;
  const std::size_t dispatch_intent_override_capacity =
      semantic_summary.effective_direct_member_sites +
      dispatch_intent_container_capacity;
  const std::size_t dispatch_intent_guard_capacity =
      dispatch_intent_callable_capacity + dispatch_intent_container_capacity;
  const std::size_t raw_guard_blocked_sites =
      legality_summary.illegal_final_superclass_sites +
      legality_summary.illegal_sealed_superclass_sites +
      legality_summary.illegal_final_override_sites +
      legality_summary.illegal_direct_override_sites +
      compatibility_summary.illegal_direct_dynamic_conflict_sites +
      compatibility_summary.illegal_final_dynamic_conflict_sites +
      compatibility_summary.illegal_non_method_callable_sites +
      compatibility_summary.illegal_protocol_method_sites +
      compatibility_summary.illegal_category_method_sites +
      compatibility_summary.illegal_category_container_sites;
  contract.direct_call_candidate_sites =
      semantic_summary.effective_direct_member_sites;
  contract.direct_members_defaulted_sites =
      semantic_summary.direct_members_defaulted_method_sites;
  contract.dynamic_opt_out_sites =
      semantic_summary.direct_members_dynamic_opt_out_sites;
  contract.final_container_sites = semantic_summary.final_container_sites;
  contract.sealed_container_sites = semantic_summary.sealed_container_sites;
  // Ordinary dynamic-override accounting must not invalidate the dispatch-intent
  // lowering contract when the source program does not opt into direct/final/sealed
  // dispatch-control features.
  contract.override_legality_sites =
      std::min(legality_summary.override_sites, dispatch_intent_override_capacity);
  contract.metadata_preserved_callable_sites =
      dispatch_intent_callable_capacity;
  contract.metadata_preserved_container_sites =
      dispatch_intent_container_capacity;
  contract.guard_blocked_sites =
      std::min(raw_guard_blocked_sites, dispatch_intent_guard_capacity);
  contract.contract_violation_sites = 0;
  contract.deterministic =
      semantic_summary.deterministic &&
      semantic_summary.ready_for_core_implementation &&
      legality_summary.deterministic &&
      legality_summary.ready_for_lowering_and_runtime &&
      compatibility_summary.deterministic &&
      compatibility_summary.ready_for_lowering_and_runtime;
  return contract;
}

Objc3TaskRuntimeInteropCancellationLoweringContract
BuildConcurrencyTaskRuntimeInteropCancellationLoweringContract(
    const Objc3ConcurrencyTaskExecutorCancellationSemanticModelSummary
        &semantic_summary,
    const Objc3ConcurrencyStructuredTaskCancellationSemanticSummary
        &structured_summary,
    const Objc3ConcurrencyExecutorHopAffinityCompatibilitySummary
        &executor_summary) {
  Objc3TaskRuntimeInteropCancellationLoweringContract contract;
  contract.task_runtime_sites =
      semantic_summary.task_runtime_interop_sites +
      structured_summary.illegal_non_async_task_sites +
      structured_summary.illegal_task_group_scope_sites +
      structured_summary.illegal_task_hierarchy_sites +
      structured_summary.illegal_cancellation_usage_sites +
      executor_summary.illegal_missing_executor_affinity_sites +
      executor_summary.illegal_main_executor_detached_sites;
  contract.task_runtime_interop_sites =
      semantic_summary.task_runtime_interop_sites;
  contract.cancellation_probe_sites =
      semantic_summary.cancellation_check_sites;
  contract.cancellation_handler_sites =
      semantic_summary.cancellation_handler_sites;
  contract.runtime_resume_sites =
      structured_summary.task_group_wait_next_sites;
  contract.runtime_cancel_sites =
      structured_summary.task_group_cancel_all_sites;
  contract.guard_blocked_sites =
      structured_summary.illegal_non_async_task_sites +
      structured_summary.illegal_task_group_scope_sites +
      structured_summary.illegal_task_hierarchy_sites +
      structured_summary.illegal_cancellation_usage_sites +
      executor_summary.illegal_missing_executor_affinity_sites +
      executor_summary.illegal_main_executor_detached_sites;
  contract.normalized_sites =
      contract.task_runtime_sites - contract.guard_blocked_sites;
  contract.contract_violation_sites = 0;
  contract.deterministic =
      semantic_summary.deterministic && structured_summary.deterministic &&
      executor_summary.deterministic &&
      semantic_summary.ready_for_lowering_and_runtime &&
      structured_summary.ready_for_lowering_and_runtime &&
      executor_summary.ready_for_lowering_and_runtime &&
      contract.task_runtime_interop_sites <= contract.task_runtime_sites;
  return contract;
}

Objc3ConcurrencyReplayRaceGuardLoweringContract
BuildConcurrencyConcurrencyReplayRaceGuardLoweringContract(
    const Objc3ConcurrencyTaskExecutorCancellationSemanticModelSummary
        &semantic_summary,
    const Objc3ConcurrencyStructuredTaskCancellationSemanticSummary
        &structured_summary,
    const Objc3ConcurrencyExecutorHopAffinityCompatibilitySummary
        &executor_summary,
    const Objc3ActorIsolationSendabilityLoweringContract &actor_contract) {
  Objc3ConcurrencyReplayRaceGuardLoweringContract contract;
  contract.replay_proof_sites =
      semantic_summary.task_creation_sites +
      structured_summary.task_group_wait_next_sites;
  contract.race_guard_sites =
      semantic_summary.cancellation_check_sites +
      semantic_summary.cancellation_handler_sites;
  contract.task_handoff_sites =
      semantic_summary.task_creation_sites +
      executor_summary.detached_task_creation_sites +
      structured_summary.task_group_wait_next_sites;
  contract.actor_isolation_sites = actor_contract.actor_isolation_sites;
  contract.guard_blocked_sites =
      structured_summary.illegal_non_async_task_sites +
      structured_summary.illegal_task_group_scope_sites +
      structured_summary.illegal_task_hierarchy_sites +
      structured_summary.illegal_cancellation_usage_sites +
      executor_summary.illegal_missing_executor_affinity_sites +
      executor_summary.illegal_main_executor_detached_sites;
  contract.deterministic_schedule_sites =
      contract.replay_proof_sites + contract.race_guard_sites +
      contract.task_handoff_sites + contract.actor_isolation_sites;
  contract.concurrency_replay_sites =
      contract.deterministic_schedule_sites + contract.guard_blocked_sites;
  contract.contract_violation_sites = 0;
  contract.deterministic =
      semantic_summary.deterministic && structured_summary.deterministic &&
      executor_summary.deterministic && actor_contract.deterministic &&
      semantic_summary.ready_for_lowering_and_runtime &&
      structured_summary.ready_for_lowering_and_runtime &&
      executor_summary.ready_for_lowering_and_runtime;
  return contract;
}

}  // namespace objc3::artifacts::frontend
