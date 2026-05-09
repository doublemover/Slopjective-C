#include "artifacts/objc3_frontend_ownership_semantic_artifacts.h"

#include <sstream>
#include <string>

#include "io/objc3_json.h"

namespace objc3::artifacts::frontend {
namespace {

using objc3::io::EscapeJsonString;

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

}  // namespace objc3::artifacts::frontend
