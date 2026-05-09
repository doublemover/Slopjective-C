#include "artifacts/objc3_frontend_ownership_semantic_artifacts.h"

#include <sstream>
#include <string>

#include "io/objc3_json.h"

namespace objc3::artifacts::frontend {
namespace {

using objc3::io::EscapeJsonString;

inline constexpr const char
    *kObjc3OwnershipBorrowedRetainableAbiCompletionArtifactModel =
        "borrowed-return-contracts-and-retainable-family-call-boundaries-now-publish-a-dedicated-ownership-abi-and-replay-packet-above-the-frozen-lowering-contract";
inline constexpr const char
    *kObjc3OwnershipBorrowedRetainableAbiCompletionProofModel =
        "the-supported-proof-slice-remains-direct-call-abi-emission-and-replay-stability-without-claiming-lane-d-runtime-helper-integration";

}  // namespace

std::string BuildEffectsOwnershipSemanticModelSummaryJson(
    const Objc3EffectsOwnershipSemanticModelSummary &summary) {
  std::ostringstream out;
  out << "{"
      << "\"contract_id\":\"" << EscapeJsonString(summary.contract_id)
      << "\",\"surface_path\":\"" << EscapeJsonString(summary.surface_path)
      << "\",\"semantic_model\":\"" << EscapeJsonString(summary.semantic_model)
      << "\",\"arc_ownership_qualified_sites\":"
      << summary.arc_ownership_qualified_sites
      << ",\"retain_insertion_sites\":" << summary.retain_insertion_sites
      << ",\"release_insertion_sites\":" << summary.release_insertion_sites
      << ",\"autorelease_insertion_sites\":"
      << summary.autorelease_insertion_sites
      << ",\"weak_zeroing_sites\":" << summary.weak_zeroing_sites
      << ",\"unowned_reference_sites\":" << summary.unowned_reference_sites
      << ",\"weak_unowned_conflict_sites\":"
      << summary.weak_unowned_conflict_sites
      << ",\"autoreleasepool_scope_sites\":"
      << summary.autoreleasepool_scope_sites
      << ",\"cleanup_order_exit_sites\":" << summary.cleanup_order_exit_sites
      << ",\"block_literal_sites\":" << summary.block_literal_sites
      << ",\"stack_to_heap_promotion_sites\":"
      << summary.stack_to_heap_promotion_sites
      << ",\"byref_forwarding_cell_sites\":"
      << summary.byref_forwarding_cell_sites
      << ",\"copy_helper_required_sites\":"
      << summary.copy_helper_required_sites
      << ",\"dispose_helper_required_sites\":"
      << summary.dispose_helper_required_sites
      << ",\"copy_helper_symbolized_sites\":"
      << summary.copy_helper_symbolized_sites
      << ",\"dispose_helper_symbolized_sites\":"
      << summary.dispose_helper_symbolized_sites
      << ",\"captured_object_lifetime_sites\":"
      << summary.captured_object_lifetime_sites
      << ",\"throws_propagation_sites\":"
      << summary.throws_propagation_sites
      << ",\"unwind_cleanup_sites\":" << summary.unwind_cleanup_sites
      << ",\"bridged_error_sites\":" << summary.bridged_error_sites
      << ",\"nested_cleanup_sites\":" << summary.nested_cleanup_sites
      << ",\"foreign_boundary_sites\":" << summary.foreign_boundary_sites
      << ",\"async_continuation_sites\":"
      << summary.async_continuation_sites
      << ",\"continuation_resume_sites\":"
      << summary.continuation_resume_sites
      << ",\"continuation_suspend_sites\":"
      << summary.continuation_suspend_sites
      << ",\"async_state_machine_sites\":"
      << summary.async_state_machine_sites
      << ",\"cancellation_propagation_sites\":"
      << summary.cancellation_propagation_sites
      << ",\"actor_isolation_sites\":" << summary.actor_isolation_sites
      << ",\"actor_hop_sites\":" << summary.actor_hop_sites
      << ",\"sendability_check_sites\":" << summary.sendability_check_sites
      << ",\"reentrancy_policy_sites\":"
      << summary.reentrancy_policy_sites
      << ",\"imported_actor_api_sites\":"
      << summary.imported_actor_api_sites
      << ",\"contract_violation_sites\":"
      << summary.contract_violation_sites
      << ",\"arc_semantics_landed\":"
      << (summary.arc_semantics_landed ? "true" : "false")
      << ",\"block_escape_semantics_landed\":"
      << (summary.block_escape_semantics_landed ? "true" : "false")
      << ",\"throws_cleanup_semantics_landed\":"
      << (summary.throws_cleanup_semantics_landed ? "true" : "false")
      << ",\"async_task_semantics_landed\":"
      << (summary.async_task_semantics_landed ? "true" : "false")
      << ",\"actor_semantics_landed\":"
      << (summary.actor_semantics_landed ? "true" : "false")
      << ",\"foreign_boundary_semantics_landed\":"
      << (summary.foreign_boundary_semantics_landed ? "true" : "false")
      << ",\"deterministic\":" << (summary.deterministic ? "true" : "false")
      << ",\"ready_for_lowering_and_runtime\":"
      << (summary.ready_for_lowering_and_runtime ? "true" : "false")
      << ",\"replay_key\":\"" << EscapeJsonString(summary.replay_key)
      << "\",\"failure_reason\":\""
      << EscapeJsonString(summary.failure_reason) << "\"}";
  return out.str();
}

Objc3OwnershipQualifierLoweringContract BuildOwnershipQualifierLoweringContract(
    const Objc3SemaParityContractSurface &sema_parity_surface) {
  Objc3OwnershipQualifierLoweringContract contract;
  contract.ownership_qualifier_sites =
      sema_parity_surface.type_annotation_ownership_qualifier_sites_total;
  contract.invalid_ownership_qualifier_sites =
      sema_parity_surface.type_annotation_invalid_ownership_qualifier_sites_total;
  contract.object_pointer_type_annotation_sites =
      sema_parity_surface.type_annotation_object_pointer_type_sites_total;
  contract.deterministic =
      sema_parity_surface.type_annotation_surface_summary.deterministic &&
      sema_parity_surface.deterministic_type_annotation_surface_handoff;
  return contract;
}

Objc3RetainReleaseOperationLoweringContract
BuildRetainReleaseOperationLoweringContract(
    const Objc3SemaParityContractSurface &sema_parity_surface) {
  Objc3RetainReleaseOperationLoweringContract contract;
  contract.ownership_qualified_sites =
      sema_parity_surface.retain_release_operation_ownership_qualified_sites_total;
  contract.retain_insertion_sites =
      sema_parity_surface.retain_release_operation_retain_insertion_sites_total;
  contract.release_insertion_sites =
      sema_parity_surface.retain_release_operation_release_insertion_sites_total;
  contract.autorelease_insertion_sites =
      sema_parity_surface
          .retain_release_operation_autorelease_insertion_sites_total;
  contract.contract_violation_sites =
      sema_parity_surface.retain_release_operation_contract_violation_sites_total;
  contract.deterministic =
      sema_parity_surface.retain_release_operation_summary.deterministic &&
      sema_parity_surface.deterministic_retain_release_operation_handoff;
  return contract;
}

Objc3AutoreleasePoolScopeLoweringContract
BuildAutoreleasePoolScopeLoweringContract(
    const Objc3SemaParityContractSurface &sema_parity_surface) {
  Objc3AutoreleasePoolScopeLoweringContract contract;
  contract.scope_sites = sema_parity_surface.autoreleasepool_scope_sites_total;
  contract.scope_symbolized_sites =
      sema_parity_surface.autoreleasepool_scope_symbolized_sites_total;
  contract.max_scope_depth =
      sema_parity_surface.autoreleasepool_scope_max_depth_total;
  contract.scope_entry_transition_sites =
      sema_parity_surface.autoreleasepool_scope_sites_total;
  contract.scope_exit_transition_sites =
      sema_parity_surface.autoreleasepool_scope_sites_total;
  contract.contract_violation_sites =
      sema_parity_surface.autoreleasepool_scope_contract_violation_sites_total;
  contract.deterministic =
      sema_parity_surface.autoreleasepool_scope_summary.deterministic &&
      sema_parity_surface.deterministic_autoreleasepool_scope_handoff;
  return contract;
}

Objc3WeakUnownedSemanticsLoweringContract
BuildWeakUnownedSemanticsLoweringContract(
    const Objc3SemaParityContractSurface &sema_parity_surface) {
  Objc3WeakUnownedSemanticsLoweringContract contract;
  contract.ownership_candidate_sites =
      sema_parity_surface.weak_unowned_semantics_ownership_candidate_sites_total;
  contract.weak_reference_sites =
      sema_parity_surface.weak_unowned_semantics_weak_reference_sites_total;
  contract.unowned_reference_sites =
      sema_parity_surface.weak_unowned_semantics_unowned_reference_sites_total;
  contract.unowned_safe_reference_sites =
      sema_parity_surface
          .weak_unowned_semantics_unowned_safe_reference_sites_total;
  contract.weak_unowned_conflict_sites =
      sema_parity_surface.weak_unowned_semantics_conflict_sites_total;
  contract.contract_violation_sites =
      sema_parity_surface.weak_unowned_semantics_contract_violation_sites_total;
  contract.deterministic =
      sema_parity_surface.weak_unowned_semantics_summary.deterministic &&
      sema_parity_surface.deterministic_weak_unowned_semantics_handoff;
  return contract;
}

Objc3ArcDiagnosticsFixitLoweringContract BuildArcDiagnosticsFixitLoweringContract(
    const Objc3SemaParityContractSurface &sema_parity_surface) {
  Objc3ArcDiagnosticsFixitLoweringContract contract;
  contract.ownership_arc_diagnostic_candidate_sites =
      sema_parity_surface.ownership_arc_diagnostic_candidate_sites_total;
  contract.ownership_arc_fixit_available_sites =
      sema_parity_surface.ownership_arc_fixit_available_sites_total;
  contract.ownership_arc_profiled_sites =
      sema_parity_surface.ownership_arc_profiled_sites_total;
  contract.ownership_arc_weak_unowned_conflict_diagnostic_sites =
      sema_parity_surface
          .ownership_arc_weak_unowned_conflict_diagnostic_sites_total;
  contract.ownership_arc_empty_fixit_hint_sites =
      sema_parity_surface.ownership_arc_empty_fixit_hint_sites_total;
  contract.contract_violation_sites =
      sema_parity_surface.ownership_arc_contract_violation_sites_total;
  contract.deterministic =
      sema_parity_surface.arc_diagnostics_fixit_summary.deterministic &&
      sema_parity_surface.deterministic_arc_diagnostics_fixit_handoff;
  return contract;
}

std::string BuildOwnershipSystemExtensionSemanticModelSummaryJson(
    const Objc3OwnershipSystemExtensionSemanticModelSummary &summary) {
  std::ostringstream out;
  out << "{"
      << "\"contract_id\":\"" << EscapeJsonString(summary.contract_id)
      << "\",\"frontend_dependency_contract_id\":\""
      << EscapeJsonString(summary.frontend_dependency_contract_id)
      << "\",\"surface_path\":\"" << EscapeJsonString(summary.surface_path)
      << "\",\"semantic_model\":\"" << EscapeJsonString(summary.semantic_model)
      << "\",\"deferred_model\":\"" << EscapeJsonString(summary.deferred_model)
      << "\",\"cleanup_attribute_sites\":" << summary.cleanup_attribute_sites
      << ",\"cleanup_sugar_sites\":" << summary.cleanup_sugar_sites
      << ",\"resource_attribute_sites\":" << summary.resource_attribute_sites
      << ",\"resource_sugar_sites\":" << summary.resource_sugar_sites
      << ",\"borrowed_pointer_sites\":" << summary.borrowed_pointer_sites
      << ",\"returns_borrowed_attribute_sites\":"
      << summary.returns_borrowed_attribute_sites
      << ",\"explicit_capture_list_sites\":"
      << summary.explicit_capture_list_sites
      << ",\"explicit_capture_item_sites\":"
      << summary.explicit_capture_item_sites
      << ",\"retainable_family_annotation_sites\":"
      << summary.retainable_family_annotation_sites
      << ",\"retainable_family_compatibility_alias_sites\":"
      << summary.retainable_family_compatibility_alias_sites
      << ",\"source_dependency_required\":"
      << (summary.source_dependency_required ? "true" : "false")
      << ",\"cleanup_resource_semantic_model_frozen\":"
      << (summary.cleanup_resource_semantic_model_frozen ? "true" : "false")
      << ",\"borrowed_pointer_semantic_model_frozen\":"
      << (summary.borrowed_pointer_semantic_model_frozen ? "true" : "false")
      << ",\"capture_legality_semantic_model_frozen\":"
      << (summary.capture_legality_semantic_model_frozen ? "true" : "false")
      << ",\"retainable_family_semantic_model_frozen\":"
      << (summary.retainable_family_semantic_model_frozen ? "true" : "false")
      << ",\"resource_move_semantics_deferred\":"
      << (summary.resource_move_semantics_deferred ? "true" : "false")
      << ",\"borrowed_escape_semantics_deferred\":"
      << (summary.borrowed_escape_semantics_deferred ? "true" : "false")
      << ",\"retainable_family_legality_deferred\":"
      << (summary.retainable_family_legality_deferred ? "true" : "false")
      << ",\"deterministic\":" << (summary.deterministic ? "true" : "false")
      << ",\"ready_for_lowering_and_runtime\":"
      << (summary.ready_for_lowering_and_runtime ? "true" : "false")
      << ",\"failure_reason\":\"" << EscapeJsonString(summary.failure_reason)
      << "\",\"replay_key\":\"" << EscapeJsonString(summary.replay_key)
      << "\"}";
  return out.str();
}

std::string BuildOwnershipResourceMoveUseAfterMoveSemanticsSummaryJson(
    const Objc3OwnershipResourceMoveUseAfterMoveSemanticsSummary &summary) {
  std::ostringstream out;
  out << "{"
      << "\"contract_id\":\"" << EscapeJsonString(summary.contract_id)
      << "\",\"dependency_contract_id\":\""
      << EscapeJsonString(summary.dependency_contract_id)
      << "\",\"surface_path\":\"" << EscapeJsonString(summary.surface_path)
      << "\",\"semantic_model\":\"" << EscapeJsonString(summary.semantic_model)
      << "\",\"deferred_model\":\"" << EscapeJsonString(summary.deferred_model)
      << "\",\"cleanup_owned_local_sites\":"
      << summary.cleanup_owned_local_sites
      << ",\"resource_move_capture_sites\":"
      << summary.resource_move_capture_sites
      << ",\"illegal_non_resource_move_sites\":"
      << summary.illegal_non_resource_move_sites
      << ",\"illegal_use_after_move_sites\":"
      << summary.illegal_use_after_move_sites
      << ",\"illegal_duplicate_move_sites\":"
      << summary.illegal_duplicate_move_sites
      << ",\"dependency_required\":"
      << (summary.dependency_required ? "true" : "false")
      << ",\"cleanup_ownership_transfer_enforced\":"
      << (summary.cleanup_ownership_transfer_enforced ? "true" : "false")
      << ",\"use_after_move_fail_closed\":"
      << (summary.use_after_move_fail_closed ? "true" : "false")
      << ",\"duplicate_move_fail_closed\":"
      << (summary.duplicate_move_fail_closed ? "true" : "false")
      << ",\"borrowed_escape_semantics_deferred\":"
      << (summary.borrowed_escape_semantics_deferred ? "true" : "false")
      << ",\"retainable_family_legality_deferred\":"
      << (summary.retainable_family_legality_deferred ? "true" : "false")
      << ",\"lowering_runtime_deferred\":"
      << (summary.lowering_runtime_deferred ? "true" : "false")
      << ",\"deterministic\":" << (summary.deterministic ? "true" : "false")
      << ",\"ready_for_lowering_and_runtime\":"
      << (summary.ready_for_lowering_and_runtime ? "true" : "false")
      << ",\"failure_reason\":\"" << EscapeJsonString(summary.failure_reason)
      << "\",\"replay_key\":\"" << EscapeJsonString(summary.replay_key)
      << "\"}";
  return out.str();
}

std::string BuildOwnershipBorrowedPointerEscapeAnalysisSummaryJson(
    const Objc3OwnershipBorrowedPointerEscapeAnalysisSummary &summary) {
  std::ostringstream out;
  out << "{"
      << "\"contract_id\":\"" << EscapeJsonString(summary.contract_id)
      << "\",\"dependency_contract_id\":\""
      << EscapeJsonString(summary.dependency_contract_id)
      << "\",\"surface_path\":\"" << EscapeJsonString(summary.surface_path)
      << "\",\"semantic_model\":\"" << EscapeJsonString(summary.semantic_model)
      << "\",\"deferred_model\":\"" << EscapeJsonString(summary.deferred_model)
      << "\",\"borrowed_parameter_sites\":" << summary.borrowed_parameter_sites
      << ",\"borrowed_return_callable_sites\":"
      << summary.borrowed_return_callable_sites
      << ",\"borrowed_escape_candidate_sites\":"
      << summary.borrowed_escape_candidate_sites
      << ",\"illegal_unproven_call_escape_sites\":"
      << summary.illegal_unproven_call_escape_sites
      << ",\"illegal_escaping_block_capture_sites\":"
      << summary.illegal_escaping_block_capture_sites
      << ",\"illegal_borrowed_return_sites\":"
      << summary.illegal_borrowed_return_sites
      << ",\"dependency_required\":"
      << (summary.dependency_required ? "true" : "false")
      << ",\"borrowed_call_boundary_enforced\":"
      << (summary.borrowed_call_boundary_enforced ? "true" : "false")
      << ",\"escaping_block_capture_fail_closed\":"
      << (summary.escaping_block_capture_fail_closed ? "true" : "false")
      << ",\"borrowed_return_contract_enforced\":"
      << (summary.borrowed_return_contract_enforced ? "true" : "false")
      << ",\"retainable_family_legality_deferred\":"
      << (summary.retainable_family_legality_deferred ? "true" : "false")
      << ",\"lowering_runtime_deferred\":"
      << (summary.lowering_runtime_deferred ? "true" : "false")
      << ",\"deterministic\":" << (summary.deterministic ? "true" : "false")
      << ",\"ready_for_lowering_and_runtime\":"
      << (summary.ready_for_lowering_and_runtime ? "true" : "false")
      << ",\"failure_reason\":\"" << EscapeJsonString(summary.failure_reason)
      << "\",\"replay_key\":\"" << EscapeJsonString(summary.replay_key)
      << "\"}";
  return out.str();
}

std::string BuildOwnershipCaptureListRetainableFamilyLegalityCompletionSummaryJson(
    const Objc3OwnershipCaptureListRetainableFamilyLegalityCompletionSummary
        &summary) {
  std::ostringstream out;
  out << "{"
      << "\"contract_id\":\"" << EscapeJsonString(summary.contract_id)
      << "\",\"dependency_contract_id\":\""
      << EscapeJsonString(summary.dependency_contract_id)
      << "\",\"surface_path\":\"" << EscapeJsonString(summary.surface_path)
      << "\",\"semantic_model\":\"" << EscapeJsonString(summary.semantic_model)
      << "\",\"deferred_model\":\"" << EscapeJsonString(summary.deferred_model)
      << "\",\"explicit_capture_list_sites\":"
      << summary.explicit_capture_list_sites
      << ",\"explicit_capture_item_sites\":"
      << summary.explicit_capture_item_sites
      << ",\"explicit_capture_ownership_mode_sites\":"
      << summary.explicit_capture_ownership_mode_sites
      << ",\"retainable_family_callable_sites\":"
      << summary.retainable_family_callable_sites
      << ",\"retainable_family_operation_callable_sites\":"
      << summary.retainable_family_operation_callable_sites
      << ",\"retainable_family_alias_callable_sites\":"
      << summary.retainable_family_alias_callable_sites
      << ",\"illegal_duplicate_explicit_capture_sites\":"
      << summary.illegal_duplicate_explicit_capture_sites
      << ",\"illegal_non_object_capture_mode_sites\":"
      << summary.illegal_non_object_capture_mode_sites
      << ",\"illegal_unused_explicit_capture_sites\":"
      << summary.illegal_unused_explicit_capture_sites
      << ",\"illegal_conflicting_retainable_family_sites\":"
      << summary.illegal_conflicting_retainable_family_sites
      << ",\"illegal_invalid_family_operation_shape_sites\":"
      << summary.illegal_invalid_family_operation_shape_sites
      << ",\"illegal_invalid_family_alias_shape_sites\":"
      << summary.illegal_invalid_family_alias_shape_sites
      << ",\"dependency_required\":"
      << (summary.dependency_required ? "true" : "false")
      << ",\"explicit_capture_duplicate_fail_closed\":"
      << (summary.explicit_capture_duplicate_fail_closed ? "true" : "false")
      << ",\"explicit_capture_ownership_mode_enforced\":"
      << (summary.explicit_capture_ownership_mode_enforced ? "true" : "false")
      << ",\"explicit_capture_inventory_enforced\":"
      << (summary.explicit_capture_inventory_enforced ? "true" : "false")
      << ",\"retainable_family_conflict_enforced\":"
      << (summary.retainable_family_conflict_enforced ? "true" : "false")
      << ",\"retainable_family_operation_shape_enforced\":"
      << (summary.retainable_family_operation_shape_enforced ? "true" : "false")
      << ",\"retainable_family_alias_shape_enforced\":"
      << (summary.retainable_family_alias_shape_enforced ? "true" : "false")
      << ",\"lowering_runtime_deferred\":"
      << (summary.lowering_runtime_deferred ? "true" : "false")
      << ",\"deterministic\":" << (summary.deterministic ? "true" : "false")
      << ",\"ready_for_lowering_and_runtime\":"
      << (summary.ready_for_lowering_and_runtime ? "true" : "false")
      << ",\"failure_reason\":\"" << EscapeJsonString(summary.failure_reason)
      << "\",\"replay_key\":\"" << EscapeJsonString(summary.replay_key)
      << "\"}";
  return out.str();
}

std::string BuildOwnershipSystemExtensionLoweringContractJson(
    const Objc3OwnershipSystemExtensionSemanticModelSummary &semantic_summary,
    const Objc3OwnershipResourceMoveUseAfterMoveSemanticsSummary &resource_summary,
    const Objc3OwnershipBorrowedPointerEscapeAnalysisSummary &borrowed_summary,
    const Objc3OwnershipCaptureListRetainableFamilyLegalityCompletionSummary
        &family_summary,
    const Objc3OwnershipSystemExtensionLoweringContract &contract,
    const std::string &replay_key) {
  const bool ready_for_ir_emission =
      contract.deterministic &&
      IsValidObjc3OwnershipSystemExtensionLoweringContract(contract);
  std::ostringstream out;
  out << "{"
      << "\"contract_id\":\""
      << EscapeJsonString(kObjc3OwnershipSystemExtensionLoweringContractId)
      << "\",\"surface_path\":\""
      << EscapeJsonString(kObjc3OwnershipSystemExtensionLoweringSurfacePath)
      << "\",\"semantic_contract_id\":\""
      << EscapeJsonString(semantic_summary.contract_id)
      << "\",\"resource_semantic_contract_id\":\""
      << EscapeJsonString(resource_summary.contract_id)
      << "\",\"borrowed_semantic_contract_id\":\""
      << EscapeJsonString(borrowed_summary.contract_id)
      << "\",\"family_semantic_contract_id\":\""
      << EscapeJsonString(family_summary.contract_id)
      << "\",\"lane_contract_id\":\""
      << EscapeJsonString(kObjc3OwnershipSystemExtensionLoweringLaneContract)
      << "\",\"lowering_model\":\""
      << EscapeJsonString(kObjc3OwnershipSystemExtensionLoweringModel)
      << "\",\"deferred_model\":\""
      << EscapeJsonString(kObjc3OwnershipSystemExtensionLoweringDeferredModel)
      << "\",\"replay_key\":\"" << EscapeJsonString(replay_key)
      << "\",\"cleanup_hook_sites\":" << contract.cleanup_hook_sites
      << ",\"resource_local_sites\":" << contract.resource_local_sites
      << ",\"cleanup_owned_local_sites\":"
      << contract.cleanup_owned_local_sites
      << ",\"resource_move_capture_sites\":"
      << contract.resource_move_capture_sites
      << ",\"borrowed_parameter_sites\":"
      << contract.borrowed_parameter_sites
      << ",\"borrowed_return_callable_sites\":"
      << contract.borrowed_return_callable_sites
      << ",\"borrowed_escape_candidate_sites\":"
      << contract.borrowed_escape_candidate_sites
      << ",\"explicit_capture_item_sites\":"
      << contract.explicit_capture_item_sites
      << ",\"retainable_family_callable_sites\":"
      << contract.retainable_family_callable_sites
      << ",\"retainable_family_operation_callable_sites\":"
      << contract.retainable_family_operation_callable_sites
      << ",\"retainable_family_alias_callable_sites\":"
      << contract.retainable_family_alias_callable_sites
      << ",\"guard_blocked_sites\":" << contract.guard_blocked_sites
      << ",\"contract_violation_sites\":"
      << contract.contract_violation_sites
      << ",\"deterministic_handoff\":"
      << (contract.deterministic ? "true" : "false")
      << ",\"ready_for_ir_emission\":"
      << (ready_for_ir_emission ? "true" : "false")
      << "}";
  return out.str();
}

std::string BuildOwnershipBorrowedRetainableAbiCompletionReplayKey(
    const Objc3OwnershipSystemExtensionLoweringContract &contract,
    const Objc3FrontendOwnershipSystemExtensionSourceClosureSummary &source_summary,
    const Objc3FrontendOwnershipRetainableCFamilySourceCompletionSummary
        &retainable_summary) {
  std::ostringstream out;
  out << "lowering_replay_key="
      << Objc3OwnershipSystemExtensionLoweringReplayKey(contract)
      << ";returns_borrowed_attribute_sites="
      << source_summary.returns_borrowed_attribute_sites
      << ";family_retain_sites=" << retainable_summary.family_retain_sites
      << ";family_release_sites=" << retainable_summary.family_release_sites
      << ";family_autorelease_sites="
      << retainable_summary.family_autorelease_sites
      << ";compatibility_returns_retained_sites="
      << retainable_summary.compatibility_returns_retained_sites
      << ";compatibility_returns_not_retained_sites="
      << retainable_summary.compatibility_returns_not_retained_sites
      << ";compatibility_consumed_sites="
      << retainable_summary.compatibility_consumed_sites
      << ";deterministic=true;lane_contract="
      << kObjc3OwnershipBorrowedRetainableAbiCompletionLaneContract;
  return out.str();
}

std::string BuildOwnershipBorrowedRetainableAbiCompletionJson(
    const Objc3OwnershipSystemExtensionLoweringContract &contract,
    const Objc3FrontendOwnershipSystemExtensionSourceClosureSummary &source_summary,
    const Objc3FrontendOwnershipRetainableCFamilySourceCompletionSummary
        &retainable_summary,
    const std::string &lowering_replay_key,
    const std::string &abi_completion_replay_key) {
  const bool ready_for_ir_emission =
      contract.deterministic &&
      IsValidObjc3OwnershipSystemExtensionLoweringContract(contract) &&
      source_summary.returns_borrowed_attribute_sites <=
          contract.borrowed_return_callable_sites &&
      retainable_summary.family_retain_sites +
              retainable_summary.family_release_sites +
              retainable_summary.family_autorelease_sites ==
          contract.retainable_family_operation_callable_sites &&
      retainable_summary.compatibility_returns_retained_sites +
              retainable_summary.compatibility_returns_not_retained_sites +
              retainable_summary.compatibility_consumed_sites ==
          contract.retainable_family_alias_callable_sites;
  std::ostringstream out;
  out << "{"
      << "\"contract_id\":\""
      << EscapeJsonString(kObjc3OwnershipBorrowedRetainableAbiCompletionContractId)
      << "\",\"surface_path\":\""
      << EscapeJsonString(kObjc3OwnershipBorrowedRetainableAbiCompletionSurfacePath)
      << "\",\"lowering_contract_id\":\""
      << EscapeJsonString(kObjc3OwnershipSystemExtensionLoweringContractId)
      << "\",\"artifact_model\":\""
      << EscapeJsonString(kObjc3OwnershipBorrowedRetainableAbiCompletionArtifactModel)
      << "\",\"proof_model\":\""
      << EscapeJsonString(kObjc3OwnershipBorrowedRetainableAbiCompletionProofModel)
      << "\",\"lowering_replay_key\":\""
      << EscapeJsonString(lowering_replay_key)
      << "\",\"replay_key\":\"" << EscapeJsonString(abi_completion_replay_key)
      << "\",\"borrowed_parameter_sites\":"
      << contract.borrowed_parameter_sites
      << ",\"borrowed_return_callable_sites\":"
      << contract.borrowed_return_callable_sites
      << ",\"returns_borrowed_attribute_sites\":"
      << source_summary.returns_borrowed_attribute_sites
      << ",\"retainable_family_callable_sites\":"
      << contract.retainable_family_callable_sites
      << ",\"retainable_family_operation_callable_sites\":"
      << contract.retainable_family_operation_callable_sites
      << ",\"retainable_family_alias_callable_sites\":"
      << contract.retainable_family_alias_callable_sites
      << ",\"family_retain_sites\":"
      << retainable_summary.family_retain_sites
      << ",\"family_release_sites\":"
      << retainable_summary.family_release_sites
      << ",\"family_autorelease_sites\":"
      << retainable_summary.family_autorelease_sites
      << ",\"compatibility_returns_retained_sites\":"
      << retainable_summary.compatibility_returns_retained_sites
      << ",\"compatibility_returns_not_retained_sites\":"
      << retainable_summary.compatibility_returns_not_retained_sites
      << ",\"compatibility_consumed_sites\":"
      << retainable_summary.compatibility_consumed_sites
      << ",\"deterministic_handoff\":true"
      << ",\"ready_for_ir_emission\":"
      << (ready_for_ir_emission ? "true" : "false")
      << "}";
  return out.str();
}

}  // namespace objc3::artifacts::frontend
