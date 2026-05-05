      << summary.interop_imported_module_name_sites
      << ",\"contract_violation_sites\":"
      << summary.contract_violation_sites
      << ",\"module_import_graph_semantics_landed\":"
      << (summary.module_import_graph_semantics_landed ? "true" : "false")
      << ",\"namespace_collision_semantics_landed\":"
      << (summary.namespace_collision_semantics_landed ? "true" : "false")
      << ",\"public_private_partition_semantics_landed\":"
      << (summary.public_private_partition_semantics_landed ? "true" : "false")
      << ",\"incremental_cache_semantics_landed\":"
      << (summary.incremental_cache_semantics_landed ? "true" : "false")
      << ",\"cross_module_conformance_semantics_landed\":"
      << (summary.cross_module_conformance_semantics_landed ? "true" : "false")
      << ",\"diagnostic_recovery_semantics_landed\":"
      << (summary.diagnostic_recovery_semantics_landed ? "true" : "false")
      << ",\"interop_import_semantics_landed\":"
      << (summary.interop_import_semantics_landed ? "true" : "false")
      << ",\"deterministic\":" << (summary.deterministic ? "true" : "false")
      << ",\"ready_for_lowering_and_runtime\":"
      << (summary.ready_for_lowering_and_runtime ? "true" : "false")
      << ",\"replay_key\":\"" << EscapeJsonString(summary.replay_key)
      << "\",\"failure_reason\":\""
      << EscapeJsonString(summary.failure_reason) << "\"}";
  return out.str();
}

std::string BuildControlFlowControlFlowSemanticModelSummaryJson(
    const Objc3ControlFlowControlFlowSemanticModelSummary &summary) {
  std::ostringstream out;
  out << "{"
      << "\"contract_id\":\"" << EscapeJsonString(summary.contract_id)
      << "\",\"frontend_dependency_contract_id\":\""
      << EscapeJsonString(summary.frontend_dependency_contract_id)
      << "\",\"surface_path\":\"" << EscapeJsonString(summary.surface_path)
      << "\",\"semantic_model\":\""
      << EscapeJsonString(summary.semantic_model)
      << "\",\"defer_model\":\"" << EscapeJsonString(summary.defer_model)
      << "\",\"match_model\":\"" << EscapeJsonString(summary.match_model)
      << "\",\"non_local_exit_model\":\""
      << EscapeJsonString(summary.non_local_exit_model)
      << "\",\"guard_binding_semantic_sites\":"
      << summary.guard_binding_semantic_sites
      << ",\"guard_binding_clause_semantic_sites\":"
      << summary.guard_binding_clause_semantic_sites
      << ",\"guard_condition_statement_sites\":"
      << summary.guard_condition_statement_sites
      << ",\"guard_condition_clause_semantic_sites\":"
      << summary.guard_condition_clause_semantic_sites
      << ",\"guard_exit_enforcement_sites\":"
      << summary.guard_exit_enforcement_sites
      << ",\"guard_refinement_sites\":" << summary.guard_refinement_sites
      << ",\"match_statement_semantic_sites\":"
      << summary.match_statement_semantic_sites
      << ",\"match_default_pattern_sites\":"
      << summary.match_default_pattern_sites
      << ",\"match_wildcard_pattern_sites\":"
      << summary.match_wildcard_pattern_sites
      << ",\"match_literal_pattern_sites\":"
      << summary.match_literal_pattern_sites
      << ",\"match_binding_scope_sites\":"
      << summary.match_binding_scope_sites
      << ",\"match_result_case_scope_sites\":"
      << summary.match_result_case_scope_sites
      << ",\"match_exhaustive_statement_sites\":"
      << summary.match_exhaustive_statement_sites
      << ",\"match_bool_exhaustive_sites\":"
      << summary.match_bool_exhaustive_sites
      << ",\"match_result_case_exhaustive_sites\":"
      << summary.match_result_case_exhaustive_sites
      << ",\"match_non_exhaustive_diagnostic_sites\":"
      << summary.match_non_exhaustive_diagnostic_sites
      << ",\"match_exhaustiveness_deferred_sites\":"
      << summary.match_exhaustiveness_deferred_sites
      << ",\"defer_statement_semantic_sites\":"
      << summary.defer_statement_semantic_sites
      << ",\"defer_scope_cleanup_order_sites\":"
      << summary.defer_scope_cleanup_order_sites
      << ",\"defer_nonlocal_exit_diagnostic_sites\":"
      << summary.defer_nonlocal_exit_diagnostic_sites
      << ",\"break_statement_sites\":" << summary.break_statement_sites
      << ",\"continue_statement_sites\":"
      << summary.continue_statement_sites
      << ",\"break_restriction_diagnostic_sites\":"
      << summary.break_restriction_diagnostic_sites
      << ",\"continue_restriction_diagnostic_sites\":"
      << summary.continue_restriction_diagnostic_sites
      << ",\"source_dependency_required\":"
      << (summary.source_dependency_required ? "true" : "false")
      << ",\"guard_refinement_semantics_landed\":"
      << (summary.guard_refinement_semantics_landed ? "true" : "false")
      << ",\"guard_exit_enforcement_landed\":"
      << (summary.guard_exit_enforcement_landed ? "true" : "false")
      << ",\"match_binding_scope_semantics_landed\":"
      << (summary.match_binding_scope_semantics_landed ? "true" : "false")
      << ",\"match_result_case_scope_semantics_landed\":"
      << (summary.match_result_case_scope_semantics_landed ? "true" : "false")
      << ",\"match_exhaustiveness_semantics_landed\":"
      << (summary.match_exhaustiveness_semantics_landed ? "true" : "false")
      << ",\"match_exhaustiveness_deferred\":"
      << (summary.match_exhaustiveness_deferred ? "true" : "false")
      << ",\"defer_cleanup_order_semantics_landed\":"
      << (summary.defer_cleanup_order_semantics_landed ? "true" : "false")
      << ",\"defer_nonlocal_exit_semantics_landed\":"
      << (summary.defer_nonlocal_exit_semantics_landed ? "true" : "false")
      << ",\"defer_cleanup_order_deferred\":"
      << (summary.defer_cleanup_order_deferred ? "true" : "false")
      << ",\"defer_nonlocal_exit_deferred\":"
      << (summary.defer_nonlocal_exit_deferred ? "true" : "false")
      << ",\"non_local_exit_restrictions_landed\":"
      << (summary.non_local_exit_restrictions_landed ? "true" : "false")
      << ",\"deterministic\":"
      << (summary.deterministic ? "true" : "false")
      << ",\"ready_for_lowering_and_runtime\":"
      << (summary.ready_for_lowering_and_runtime ? "true" : "false")
      << ",\"failure_reason\":\""
      << EscapeJsonString(summary.failure_reason)
      << "\""
      << ",\"replay_key\":\"" << EscapeJsonString(summary.replay_key)
      << "\"}";
  return out.str();
}

std::string BuildErrorHandlingErrorSemanticModelSummaryJson(
    const Objc3ErrorHandlingErrorSemanticModelSummary &summary) {
  std::ostringstream out;
  out << "{"
      << "\"contract_id\":\"" << EscapeJsonString(summary.contract_id)
      << "\",\"frontend_dependency_contract_id\":\""
      << EscapeJsonString(summary.frontend_dependency_contract_id)
      << "\",\"surface_path\":\"" << EscapeJsonString(summary.surface_path)
      << "\",\"semantic_model\":\""
      << EscapeJsonString(summary.semantic_model)
      << "\",\"deferred_model\":\""
      << EscapeJsonString(summary.deferred_model)
      << "\",\"throws_declaration_sites\":" << summary.throws_declaration_sites
      << ",\"function_throws_declaration_sites\":"
      << summary.function_throws_declaration_sites
      << ",\"method_throws_declaration_sites\":"
      << summary.method_throws_declaration_sites
      << ",\"result_like_sites\":" << summary.result_like_sites
      << ",\"result_success_sites\":" << summary.result_success_sites
      << ",\"result_failure_sites\":" << summary.result_failure_sites
      << ",\"result_branch_sites\":" << summary.result_branch_sites
      << ",\"result_payload_sites\":" << summary.result_payload_sites
      << ",\"ns_error_bridging_sites\":" << summary.ns_error_bridging_sites
      << ",\"ns_error_out_parameter_sites\":"
      << summary.ns_error_out_parameter_sites
      << ",\"ns_error_bridge_path_sites\":"
      << summary.ns_error_bridge_path_sites
      << ",\"objc_nserror_attribute_sites\":"
      << summary.objc_nserror_attribute_sites
      << ",\"objc_status_code_attribute_sites\":"
      << summary.objc_status_code_attribute_sites
      << ",\"status_code_success_clause_sites\":"
      << summary.status_code_success_clause_sites
      << ",\"status_code_error_type_clause_sites\":"
      << summary.status_code_error_type_clause_sites
      << ",\"status_code_mapping_clause_sites\":"
      << summary.status_code_mapping_clause_sites
      << ",\"placeholder_throws_propagation_sites\":"
      << summary.placeholder_throws_propagation_sites
      << ",\"placeholder_unwind_cleanup_sites\":"
      << summary.placeholder_unwind_cleanup_sites
      << ",\"source_dependency_required\":"
      << (summary.source_dependency_required ? "true" : "false")
      << ",\"throws_declaration_semantics_landed\":"
      << (summary.throws_declaration_semantics_landed ? "true" : "false")
      << ",\"result_carrier_profile_semantics_landed\":"
      << (summary.result_carrier_profile_semantics_landed ? "true" : "false")
      << ",\"ns_error_bridging_profile_semantics_landed\":"
      << (summary.ns_error_bridging_profile_semantics_landed ? "true" : "false")
      << ",\"bridge_marker_semantics_landed\":"
      << (summary.bridge_marker_semantics_landed ? "true" : "false")
      << ",\"parser_fail_closed_boundary_required\":"
      << (summary.parser_fail_closed_boundary_required ? "true" : "false")
      << ",\"parser_fail_closed_boundary_preserved\":"
      << (summary.parser_fail_closed_boundary_preserved ? "true" : "false")
      << ",\"propagation_runtime_deferred\":"
      << (summary.propagation_runtime_deferred ? "true" : "false")
      << ",\"status_to_error_runtime_deferred\":"
      << (summary.status_to_error_runtime_deferred ? "true" : "false")
      << ",\"native_error_abi_deferred\":"
      << (summary.native_error_abi_deferred ? "true" : "false")
      << ",\"placeholder_throws_summary_carried\":"
      << (summary.placeholder_throws_summary_carried ? "true" : "false")
      << ",\"deterministic\":" << (summary.deterministic ? "true" : "false")
      << ",\"ready_for_lowering_and_runtime\":"
      << (summary.ready_for_lowering_and_runtime ? "true" : "false")
      << ",\"failure_reason\":\"" << EscapeJsonString(summary.failure_reason)
      << "\",\"replay_key\":\"" << EscapeJsonString(summary.replay_key)
      << "\"}";
  return out.str();
}

std::string BuildConcurrencyTaskExecutorCancellationSemanticModelSummaryJson(
    const Objc3ConcurrencyTaskExecutorCancellationSemanticModelSummary &summary) {
  std::ostringstream out;
  out << "{"
      << "\"contract_id\":\"" << EscapeJsonString(summary.contract_id)
      << "\",\"frontend_dependency_contract_id\":\""
      << EscapeJsonString(summary.frontend_dependency_contract_id)
      << "\",\"surface_path\":\"" << EscapeJsonString(summary.surface_path)
      << "\",\"semantic_model\":\"" << EscapeJsonString(summary.semantic_model)
      << "\",\"deferred_model\":\"" << EscapeJsonString(summary.deferred_model)
      << "\",\"async_callable_sites\":" << summary.async_callable_sites
      << ",\"executor_attribute_sites\":" << summary.executor_attribute_sites
      << ",\"task_creation_sites\":" << summary.task_creation_sites
      << ",\"task_group_scope_sites\":" << summary.task_group_scope_sites
      << ",\"task_group_add_task_sites\":" << summary.task_group_add_task_sites
      << ",\"task_group_wait_next_sites\":" << summary.task_group_wait_next_sites
      << ",\"task_group_cancel_all_sites\":" << summary.task_group_cancel_all_sites
      << ",\"task_runtime_interop_sites\":" << summary.task_runtime_interop_sites
      << ",\"runtime_hook_sites\":" << summary.runtime_hook_sites
      << ",\"cancellation_check_sites\":" << summary.cancellation_check_sites
      << ",\"cancellation_handler_sites\":" << summary.cancellation_handler_sites
      << ",\"suspension_point_sites\":" << summary.suspension_point_sites
      << ",\"cancellation_propagation_sites\":"
      << summary.cancellation_propagation_sites
      << ",\"source_dependency_required\":"
      << (summary.source_dependency_required ? "true" : "false")
      << ",\"task_lifetime_semantics_landed\":"
      << (summary.task_lifetime_semantics_landed ? "true" : "false")
      << ",\"executor_affinity_semantics_landed\":"
      << (summary.executor_affinity_semantics_landed ? "true" : "false")
      << ",\"cancellation_observation_semantics_landed\":"
      << (summary.cancellation_observation_semantics_landed ? "true" : "false")
      << ",\"structured_task_legality_semantics_landed\":"
      << (summary.structured_task_legality_semantics_landed ? "true" : "false")
      << ",\"runnable_lowering_deferred\":"
      << (summary.runnable_lowering_deferred ? "true" : "false")
      << ",\"executor_runtime_deferred\":"
      << (summary.executor_runtime_deferred ? "true" : "false")
      << ",\"scheduler_runtime_deferred\":"
      << (summary.scheduler_runtime_deferred ? "true" : "false")
      << ",\"deterministic\":" << (summary.deterministic ? "true" : "false")
      << ",\"ready_for_lowering_and_runtime\":"
      << (summary.ready_for_lowering_and_runtime ? "true" : "false")
      << ",\"failure_reason\":\"" << EscapeJsonString(summary.failure_reason)
      << "\",\"replay_key\":\"" << EscapeJsonString(summary.replay_key)
      << "\"}";
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

std::string BuildConcurrencyActorIsolationSendableSemanticModelSummaryJson(
    const Objc3ConcurrencyActorIsolationSendableSemanticModelSummary &summary) {
  std::ostringstream out;
  out << "{"
      << "\"contract_id\":\"" << EscapeJsonString(summary.contract_id)
      << "\",\"frontend_dependency_contract_id\":\""
      << EscapeJsonString(summary.frontend_dependency_contract_id)
      << "\",\"surface_path\":\"" << EscapeJsonString(summary.surface_path)
      << "\",\"semantic_model\":\"" << EscapeJsonString(summary.semantic_model)
      << "\",\"deferred_model\":\"" << EscapeJsonString(summary.deferred_model)
      << "\",\"actor_interface_sites\":" << summary.actor_interface_sites
      << ",\"actor_method_sites\":" << summary.actor_method_sites
      << ",\"actor_property_sites\":" << summary.actor_property_sites
      << ",\"objc_nonisolated_annotation_sites\":"
      << summary.objc_nonisolated_annotation_sites
      << ",\"actor_member_executor_annotation_sites\":"
      << summary.actor_member_executor_annotation_sites
      << ",\"actor_async_method_sites\":"
      << summary.actor_async_method_sites
      << ",\"actor_member_metadata_sites\":"
      << summary.actor_member_metadata_sites
      << ",\"actor_isolation_sendability_sites\":"
      << summary.actor_isolation_sendability_sites
      << ",\"actor_isolation_decl_sites\":"
      << summary.actor_isolation_decl_sites
      << ",\"actor_hop_sites\":" << summary.actor_hop_sites
      << ",\"sendable_annotation_sites\":"
      << summary.sendable_annotation_sites
      << ",\"non_sendable_crossing_sites\":"
      << summary.non_sendable_crossing_sites
      << ",\"isolation_boundary_sites\":"
      << summary.isolation_boundary_sites
      << ",\"normalized_sites\":" << summary.normalized_sites
      << ",\"gate_blocked_sites\":" << summary.gate_blocked_sites
      << ",\"contract_violation_sites\":"
      << summary.contract_violation_sites
      << ",\"source_dependency_required\":"
      << (summary.source_dependency_required ? "true" : "false")
      << ",\"actor_member_source_supported\":"
      << (summary.actor_member_source_supported ? "true" : "false")
      << ",\"actor_isolation_sendability_profile_normalized\":"
      << (summary.actor_isolation_sendability_profile_normalized ? "true"
                                                                 : "false")
      << ",\"strict_concurrency_selection_fail_closed\":"
      << (summary.strict_concurrency_selection_fail_closed ? "true" : "false")
      << ",\"actor_runtime_deferred\":"
      << (summary.actor_runtime_deferred ? "true" : "false")
      << ",\"executor_runtime_deferred\":"
      << (summary.executor_runtime_deferred ? "true" : "false")
      << ",\"cross_actor_enforcement_deferred\":"
      << (summary.cross_actor_enforcement_deferred ? "true" : "false")
      << ",\"deterministic\":" << (summary.deterministic ? "true" : "false")
      << ",\"ready_for_semantic_expansion\":"
      << (summary.ready_for_semantic_expansion ? "true" : "false")
      << ",\"failure_reason\":\""
      << EscapeJsonString(summary.failure_reason)
      << "\",\"replay_key\":\"" << EscapeJsonString(summary.replay_key)
      << "\"}";
  return out.str();
}

std::string BuildConcurrencyActorIsolationSendabilityEnforcementSummaryJson(
    const Objc3ConcurrencyActorIsolationSendabilityEnforcementSummary &summary) {
  std::ostringstream out;
  out << "{"
      << "\"contract_id\":\"" << EscapeJsonString(summary.contract_id)
      << "\",\"dependency_contract_id\":\""
      << EscapeJsonString(summary.dependency_contract_id)
      << "\",\"surface_path\":\"" << EscapeJsonString(summary.surface_path)
      << "\",\"semantic_model\":\"" << EscapeJsonString(summary.semantic_model)
      << "\",\"deferred_model\":\"" << EscapeJsonString(summary.deferred_model)
      << "\",\"actor_interface_sites\":" << summary.actor_interface_sites
      << ",\"actor_method_sites\":" << summary.actor_method_sites
      << ",\"objc_nonisolated_annotation_sites\":"
      << summary.objc_nonisolated_annotation_sites
      << ",\"actor_member_executor_annotation_sites\":"
      << summary.actor_member_executor_annotation_sites
      << ",\"actor_async_method_sites\":"
      << summary.actor_async_method_sites
      << ",\"actor_hop_sites\":" << summary.actor_hop_sites
      << ",\"non_sendable_crossing_sites\":"
      << summary.non_sendable_crossing_sites
      << ",\"total_nonisolated_method_sites\":"
      << summary.total_nonisolated_method_sites
      << ",\"illegal_non_actor_nonisolated_sites\":"
      << summary.illegal_non_actor_nonisolated_sites
      << ",\"illegal_nonisolated_async_sites\":"
      << summary.illegal_nonisolated_async_sites
      << ",\"illegal_nonisolated_executor_sites\":"
      << summary.illegal_nonisolated_executor_sites
      << ",\"illegal_actor_hop_without_async_sites\":"
      << summary.illegal_actor_hop_without_async_sites
      << ",\"illegal_non_sendable_crossing_sites\":"
      << summary.illegal_non_sendable_crossing_sites
      << ",\"dependency_required\":"
      << (summary.dependency_required ? "true" : "false")
      << ",\"non_actor_nonisolated_fail_closed\":"
      << (summary.non_actor_nonisolated_fail_closed ? "true" : "false")
      << ",\"nonisolated_combination_fail_closed\":"
      << (summary.nonisolated_combination_fail_closed ? "true" : "false")
      << ",\"actor_hop_async_boundary_enforced\":"
      << (summary.actor_hop_async_boundary_enforced ? "true" : "false")
      << ",\"non_sendable_crossing_fail_closed\":"
      << (summary.non_sendable_crossing_fail_closed ? "true" : "false")
      << ",\"runnable_lowering_deferred\":"
      << (summary.runnable_lowering_deferred ? "true" : "false")
      << ",\"actor_runtime_deferred\":"
      << (summary.actor_runtime_deferred ? "true" : "false")
      << ",\"executor_runtime_deferred\":"
      << (summary.executor_runtime_deferred ? "true" : "false")
      << ",\"deterministic\":" << (summary.deterministic ? "true" : "false")
      << ",\"ready_for_lowering_and_runtime\":"
      << (summary.ready_for_lowering_and_runtime ? "true" : "false")
      << ",\"failure_reason\":\""
      << EscapeJsonString(summary.failure_reason)
      << "\",\"replay_key\":\"" << EscapeJsonString(summary.replay_key)
      << "\"}";
  return out.str();
}

std::string BuildConcurrencyActorRaceHazardEscapeDiagnosticsSummaryJson(
    const Objc3ConcurrencyActorRaceHazardEscapeDiagnosticsSummary &summary) {
  std::ostringstream out;
  out << "{"
      << "\"contract_id\":\"" << EscapeJsonString(summary.contract_id)
      << "\",\"dependency_contract_id\":\""
      << EscapeJsonString(summary.dependency_contract_id)
      << "\",\"surface_path\":\"" << EscapeJsonString(summary.surface_path)
      << "\",\"semantic_model\":\"" << EscapeJsonString(summary.semantic_model)
      << "\",\"deferred_model\":\"" << EscapeJsonString(summary.deferred_model)
      << "\",\"actor_method_sites\":" << summary.actor_method_sites
      << ",\"replay_proof_sites\":" << summary.replay_proof_sites
      << ",\"race_guard_sites\":" << summary.race_guard_sites
      << ",\"task_handoff_sites\":" << summary.task_handoff_sites
      << ",\"actor_isolation_sites\":" << summary.actor_isolation_sites
      << ",\"escaping_block_literal_sites\":"
      << summary.escaping_block_literal_sites
      << ",\"illegal_missing_race_guard_sites\":"
      << summary.illegal_missing_race_guard_sites
      << ",\"illegal_missing_replay_proof_sites\":"
      << summary.illegal_missing_replay_proof_sites
      << ",\"illegal_missing_actor_isolation_sites\":"
      << summary.illegal_missing_actor_isolation_sites
      << ",\"illegal_escaping_block_literal_sites\":"
      << summary.illegal_escaping_block_literal_sites
      << ",\"dependency_required\":"
      << (summary.dependency_required ? "true" : "false")
      << ",\"race_guard_fail_closed\":"
      << (summary.race_guard_fail_closed ? "true" : "false")
      << ",\"replay_proof_fail_closed\":"
      << (summary.replay_proof_fail_closed ? "true" : "false")
      << ",\"actor_isolation_boundary_fail_closed\":"
      << (summary.actor_isolation_boundary_fail_closed ? "true" : "false")
      << ",\"escaping_block_fail_closed\":"
      << (summary.escaping_block_fail_closed ? "true" : "false")
      << ",\"runnable_lowering_deferred\":"
      << (summary.runnable_lowering_deferred ? "true" : "false")
      << ",\"actor_runtime_deferred\":"
      << (summary.actor_runtime_deferred ? "true" : "false")
      << ",\"deterministic\":" << (summary.deterministic ? "true" : "false")
      << ",\"ready_for_lowering_and_runtime\":"
      << (summary.ready_for_lowering_and_runtime ? "true" : "false")
      << ",\"failure_reason\":\""
      << EscapeJsonString(summary.failure_reason)
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

std::string BuildConcurrencyStructuredTaskCancellationSemanticSummaryJson(
    const Objc3ConcurrencyStructuredTaskCancellationSemanticSummary &summary) {
  std::ostringstream out;
  out << "{"
      << "\"contract_id\":\"" << EscapeJsonString(summary.contract_id)
      << "\",\"dependency_contract_id\":\""
      << EscapeJsonString(summary.dependency_contract_id)
      << "\",\"surface_path\":\"" << EscapeJsonString(summary.surface_path)
      << "\",\"semantic_model\":\"" << EscapeJsonString(summary.semantic_model)
      << "\",\"deferred_model\":\"" << EscapeJsonString(summary.deferred_model)
      << "\",\"async_callable_sites\":" << summary.async_callable_sites
      << ",\"task_creation_sites\":" << summary.task_creation_sites
      << ",\"task_group_scope_sites\":" << summary.task_group_scope_sites
      << ",\"task_group_add_task_sites\":" << summary.task_group_add_task_sites
      << ",\"task_group_wait_next_sites\":"
      << summary.task_group_wait_next_sites
      << ",\"task_group_cancel_all_sites\":"
      << summary.task_group_cancel_all_sites
      << ",\"cancellation_check_sites\":"
      << summary.cancellation_check_sites
      << ",\"cancellation_handler_sites\":"
      << summary.cancellation_handler_sites
      << ",\"illegal_non_async_task_sites\":"
      << summary.illegal_non_async_task_sites
      << ",\"illegal_task_group_scope_sites\":"
      << summary.illegal_task_group_scope_sites
      << ",\"illegal_task_hierarchy_sites\":"
      << summary.illegal_task_hierarchy_sites
      << ",\"illegal_cancellation_usage_sites\":"
      << summary.illegal_cancellation_usage_sites
      << ",\"source_dependency_required\":"
      << (summary.source_dependency_required ? "true" : "false")
      << ",\"async_task_boundary_enforced\":"
      << (summary.async_task_boundary_enforced ? "true" : "false")
      << ",\"structured_task_scope_enforced\":"
      << (summary.structured_task_scope_enforced ? "true" : "false")
      << ",\"task_hierarchy_enforced\":"
      << (summary.task_hierarchy_enforced ? "true" : "false")
      << ",\"cancellation_usage_enforced\":"
      << (summary.cancellation_usage_enforced ? "true" : "false")
      << ",\"runnable_lowering_deferred\":"
      << (summary.runnable_lowering_deferred ? "true" : "false")
      << ",\"executor_runtime_deferred\":"
      << (summary.executor_runtime_deferred ? "true" : "false")
      << ",\"scheduler_runtime_deferred\":"
      << (summary.scheduler_runtime_deferred ? "true" : "false")
      << ",\"deterministic\":" << (summary.deterministic ? "true" : "false")
      << ",\"ready_for_lowering_and_runtime\":"
      << (summary.ready_for_lowering_and_runtime ? "true" : "false")
      << ",\"failure_reason\":\"" << EscapeJsonString(summary.failure_reason)
      << "\",\"replay_key\":\"" << EscapeJsonString(summary.replay_key)
      << "\"}";
  return out.str();
}

std::string BuildConcurrencyExecutorHopAffinityCompatibilitySummaryJson(
    const Objc3ConcurrencyExecutorHopAffinityCompatibilitySummary &summary) {
  std::ostringstream out;
  out << "{"
      << "\"contract_id\":\"" << EscapeJsonString(summary.contract_id)
      << "\",\"dependency_contract_id\":\""
      << EscapeJsonString(summary.dependency_contract_id)
      << "\",\"surface_path\":\"" << EscapeJsonString(summary.surface_path)
      << "\",\"semantic_model\":\"" << EscapeJsonString(summary.semantic_model)
      << "\",\"deferred_model\":\"" << EscapeJsonString(summary.deferred_model)
      << "\",\"async_callable_sites\":" << summary.async_callable_sites
      << ",\"executor_affinity_sites\":" << summary.executor_affinity_sites
      << ",\"executor_main_sites\":" << summary.executor_main_sites
      << ",\"executor_global_sites\":" << summary.executor_global_sites
      << ",\"executor_named_sites\":" << summary.executor_named_sites
      << ",\"task_creation_sites\":" << summary.task_creation_sites
      << ",\"detached_task_creation_sites\":"
      << summary.detached_task_creation_sites
      << ",\"illegal_missing_executor_affinity_sites\":"
      << summary.illegal_missing_executor_affinity_sites
      << ",\"illegal_main_executor_detached_sites\":"
      << summary.illegal_main_executor_detached_sites
      << ",\"dependency_required\":"
      << (summary.dependency_required ? "true" : "false")
      << ",\"executor_affinity_required_for_task_callables_enforced\":"
      << (summary.executor_affinity_required_for_task_callables_enforced
              ? "true"
              : "false")
      << ",\"detached_task_hop_boundary_enforced\":"
      << (summary.detached_task_hop_boundary_enforced ? "true" : "false")
      << ",\"runnable_lowering_deferred\":"
      << (summary.runnable_lowering_deferred ? "true" : "false")
      << ",\"executor_runtime_deferred\":"
      << (summary.executor_runtime_deferred ? "true" : "false")
      << ",\"scheduler_runtime_deferred\":"
      << (summary.scheduler_runtime_deferred ? "true" : "false")
      << ",\"deterministic\":" << (summary.deterministic ? "true" : "false")
      << ",\"ready_for_lowering_and_runtime\":"
      << (summary.ready_for_lowering_and_runtime ? "true" : "false")
      << ",\"failure_reason\":\"" << EscapeJsonString(summary.failure_reason)
      << "\",\"replay_key\":\"" << EscapeJsonString(summary.replay_key)
      << "\"}";
  return out.str();
}

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

Objc3ActorLoweringMetadataContract BuildConcurrencyActorLoweringMetadataContract(
    const Objc3FrontendConcurrencyActorMemberIsolationSourceClosureSummary
        &source_summary,
    const Objc3ConcurrencyActorIsolationSendabilityEnforcementSummary
        &enforcement_summary,
    const Objc3ConcurrencyActorRaceHazardEscapeDiagnosticsSummary &hazard_summary) {
  Objc3ActorLoweringMetadataContract contract;
  contract.actor_interface_sites = source_summary.actor_interface_sites;
  contract.actor_method_sites = enforcement_summary.actor_method_sites;
  contract.actor_metadata_record_sites =
      source_summary.actor_interface_sites +
      source_summary.actor_member_metadata_sites;
  contract.nonisolated_entry_sites =
      enforcement_summary.total_nonisolated_method_sites;
  contract.executor_affinity_sites =
      enforcement_summary.actor_member_executor_annotation_sites;
  contract.actor_hop_artifact_sites = enforcement_summary.actor_hop_sites;
  contract.actor_isolation_thunk_sites = hazard_summary.task_handoff_sites;
  contract.replay_proof_dependency_sites = hazard_summary.replay_proof_sites;
  contract.race_guard_dependency_sites = hazard_summary.race_guard_sites;
  contract.task_handoff_sites = hazard_summary.task_handoff_sites;
  contract.guard_blocked_sites =
      enforcement_summary.illegal_non_actor_nonisolated_sites +
      enforcement_summary.illegal_nonisolated_async_sites +
      enforcement_summary.illegal_nonisolated_executor_sites +
      enforcement_summary.illegal_actor_hop_without_async_sites +
      enforcement_summary.illegal_non_sendable_crossing_sites +
      hazard_summary.illegal_missing_race_guard_sites +
      hazard_summary.illegal_missing_replay_proof_sites +
      hazard_summary.illegal_missing_actor_isolation_sites +
      hazard_summary.illegal_escaping_block_literal_sites;
  contract.contract_violation_sites = 0;
  contract.deterministic =
      source_summary.deterministic_handoff &&
      source_summary.ready_for_semantic_expansion &&
      enforcement_summary.deterministic &&
      enforcement_summary.ready_for_lowering_and_runtime &&
      hazard_summary.deterministic &&
      hazard_summary.ready_for_lowering_and_runtime;
  return contract;
}

Objc3OwnershipSystemExtensionLoweringContract
BuildOwnershipSystemExtensionLoweringContract(
    const Objc3OwnershipSystemExtensionSemanticModelSummary &semantic_summary,
    const Objc3OwnershipResourceMoveUseAfterMoveSemanticsSummary &resource_summary,
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
    const Objc3DispatchDispatchIntentCompatibilitySummary &compatibility_summary) {
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

Objc3MetaprogrammingExpansionLoweringContract BuildMetaprogrammingExpansionLoweringContract(
    const Objc3FrontendMetaprogrammingPropertyBehaviorSourceCompletionSummary
        &property_source_summary,
    const Objc3MetaprogrammingDeriveExpansionInventorySummary &derive_summary,
    const Objc3MetaprogrammingMacroSafetySandboxDeterminismSummary &macro_summary,
    const Objc3MetaprogrammingPropertyBehaviorLegalityCompatibilitySummary
        &property_legality_summary) {
  Objc3MetaprogrammingExpansionLoweringContract contract;
  contract.derive_inventory_sites = derive_summary.supported_derive_request_sites;
  contract.derived_selector_artifact_sites =
      derive_summary.generated_method_entry_count;
  contract.macro_replay_visible_sites = macro_summary.expansion_visible_macro_sites;
  contract.property_behavior_sites = property_legality_summary.property_behavior_sites;
  contract.synthesized_binding_sites =
      property_source_summary.synthesized_binding_visible_sites;
  contract.synthesized_getter_sites =
      property_source_summary.synthesized_getter_visible_sites;
  contract.synthesized_setter_sites =
      property_source_summary.synthesized_setter_visible_sites;
  contract.replay_visible_metadata_sites =
      contract.derived_selector_artifact_sites +
      contract.macro_replay_visible_sites + contract.property_behavior_sites +
      contract.synthesized_binding_sites + contract.synthesized_getter_sites +
      contract.synthesized_setter_sites;
  contract.guard_blocked_sites =
      derive_summary.unsupported_derive_request_sites +
      derive_summary.unsupported_topology_sites +
      derive_summary.selector_conflict_sites +
      macro_summary.incomplete_macro_metadata_sites +
      macro_summary.orphan_macro_metadata_sites +
      macro_summary.invalid_package_sites +
      macro_summary.invalid_provenance_sites +
      macro_summary.nondeterministic_callable_sites +
      macro_summary.unsupported_callable_topology_sites +
      property_legality_summary.unsupported_behavior_sites +
      property_legality_summary.observed_on_protocol_sites +
      property_legality_summary.observed_readonly_conflict_sites +
      property_legality_summary.projected_writable_conflict_sites +
      property_legality_summary.non_object_behavior_sites;
  contract.contract_violation_sites = 0;
  contract.deterministic =
      derive_summary.deterministic &&
      derive_summary.ready_for_lowering_and_runtime &&
      macro_summary.deterministic &&
      macro_summary.ready_for_lowering_and_runtime &&
      property_legality_summary.deterministic &&
      property_legality_summary.ready_for_lowering_and_runtime &&
      property_source_summary.deterministic_handoff &&
      property_source_summary.ready_for_semantic_expansion;
  return contract;
}

Objc3InteropInteropLoweringContract BuildInteropInteropLoweringContract(
    const Objc3InteropInteropSemanticModelSummary &semantic_summary,
    const Objc3InteropInteropRuntimeParitySummary &runtime_parity_summary,
    const Objc3InteropCppInteropInteractionSummary &cpp_summary,
    const Objc3InteropSwiftInteropIsolationSummary &swift_summary,
    const Objc3InteropForeignSurfaceInterfacePreservationSummary
        &preservation_summary) {
  Objc3InteropInteropLoweringContract contract;
  contract.foreign_callable_sites = semantic_summary.foreign_callable_sites;
  contract.c_foreign_callable_sites = runtime_parity_summary.c_foreign_callable_sites;
  contract.objc_runtime_parity_callable_sites =
      runtime_parity_summary.objc_runtime_parity_callable_sites;
  contract.ownership_bridge_callable_sites =
      semantic_summary.bridge_callable_sites +
      cpp_summary.ownership_interaction_sites;
  contract.error_surface_sites = cpp_summary.throws_interaction_sites;
  contract.async_boundary_sites =
      semantic_summary.async_executor_affinity_sites +
      cpp_summary.async_interaction_sites;
  contract.swift_concurrency_metadata_sites =
      swift_summary.actor_owned_swift_callable_sites +
      swift_summary.nonisolated_swift_callable_sites;
  contract.interface_preserved_foreign_callable_sites =
      preservation_summary.local_foreign_callable_count +
      preservation_summary.imported_foreign_callable_count;
  contract.interface_preserved_metadata_annotation_sites =
      preservation_summary.local_import_module_annotation_count +
      preservation_summary.local_imported_module_name_count +
      preservation_summary.local_swift_name_annotation_count +
      preservation_summary.local_swift_private_annotation_count +
      preservation_summary.local_cpp_name_annotation_count +
      preservation_summary.local_header_name_annotation_count +
      preservation_summary.local_named_annotation_payload_count +
      preservation_summary.imported_import_module_annotation_count +
      preservation_summary.imported_imported_module_name_count +
      preservation_summary.imported_swift_name_annotation_count +
      preservation_summary.imported_swift_private_annotation_count +
      preservation_summary.imported_cpp_name_annotation_count +
      preservation_summary.imported_header_name_annotation_count +
      preservation_summary.imported_named_annotation_payload_count;
  contract.guard_blocked_sites =
      runtime_parity_summary.foreign_definition_rejection_sites +
      runtime_parity_summary.import_without_foreign_rejection_sites +
      runtime_parity_summary.implementation_annotation_rejection_sites +
      cpp_summary.ownership_rejection_sites + cpp_summary.throws_rejection_sites +
      cpp_summary.async_rejection_sites +
      swift_summary.swift_private_without_name_rejection_sites +
      swift_summary.actor_isolation_mapping_rejection_sites +
      swift_summary.nonisolated_mapping_rejection_sites +
      swift_summary.implementation_surface_rejection_sites;
  contract.contract_violation_sites = 0;
  contract.deterministic =
      semantic_summary.deterministic &&
      semantic_summary.ready_for_semantic_expansion &&
      runtime_parity_summary.deterministic &&
      runtime_parity_summary.ready_for_lowering_and_runtime &&
      cpp_summary.deterministic &&
      cpp_summary.ready_for_lowering_and_runtime &&
      swift_summary.deterministic &&
      swift_summary.ready_for_lowering_and_runtime &&
      preservation_summary.deterministic &&
      preservation_summary.runtime_import_artifact_ready &&
      preservation_summary.separate_compilation_preservation_ready;
  return contract;
}

Objc3InteropForeignCallLifetimeLoweringContract
BuildInteropForeignCallLifetimeLoweringContract(
    const Objc3Program &program,
    const Objc3InteropInteropLoweringContract &dependency_contract,
    const Objc3InteropCppInteropInteractionSummary &cpp_summary,
    const Objc3InteropForeignSurfaceInterfacePreservationSummary
        &preservation_summary) {
  Objc3InteropForeignCallLifetimeLoweringContract contract;
  contract.foreign_callable_sites = dependency_contract.foreign_callable_sites;
  contract.c_foreign_callable_sites =
      dependency_contract.c_foreign_callable_sites;
  contract.objc_runtime_parity_callable_sites =
      dependency_contract.objc_runtime_parity_callable_sites;

  const auto callable_has_metadata = [](const auto &decl) {
    return decl.objc_foreign_declared || decl.objc_import_module_declared ||
           decl.objc_cxx_name_declared || decl.objc_header_name_declared ||
           decl.objc_swift_name_declared || decl.objc_swift_private_declared ||
           decl.objc_export_header_declared || decl.objc_abi_align_declared ||
           decl.objc_foreign_type_declared || decl.objc_mixed_image_declared ||
           decl.objc_package_entry_declared;
  };
  const auto callable_has_lifetime_bridge = [](const auto &decl) {
    if (!decl.return_ownership_lifetime_profile.empty()) {
      return true;
    }
    return std::any_of(decl.params.begin(), decl.params.end(),
                       [](const auto &param) {
                         return !param.ownership_lifetime_profile.empty();
                       });
  };
  const auto callable_has_ownership_bridge = [](const auto &decl) {
    if (decl.return_ownership_insert_retain ||
        decl.return_ownership_insert_release ||
        decl.return_ownership_insert_autorelease) {
      return true;
    }
    return std::any_of(decl.params.begin(), decl.params.end(),
                       [](const auto &param) {
                         return param.ownership_insert_retain ||
                                param.ownership_insert_release ||
                                param.ownership_insert_autorelease;
                       });
  };

  for (const auto &fn : program.functions) {
    if (callable_has_metadata(fn)) {
      ++contract.metadata_preservation_sites;
    }
    if ((fn.objc_cxx_name_declared || fn.objc_swift_name_declared ||
         fn.objc_swift_private_declared) &&
        callable_has_ownership_bridge(fn)) {
      ++contract.ownership_bridge_sites;
    }
    if ((fn.objc_cxx_name_declared || fn.objc_swift_name_declared ||
         fn.objc_swift_private_declared) &&
        callable_has_lifetime_bridge(fn)) {
      ++contract.lifetime_bridge_sites;
    }
  }

  contract.guard_blocked_sites =
      dependency_contract.guard_blocked_sites +
      cpp_summary.ownership_rejection_sites + cpp_summary.throws_rejection_sites +
      cpp_summary.async_rejection_sites;
  contract.contract_violation_sites = dependency_contract.contract_violation_sites;
  contract.deterministic =
      IsValidObjc3InteropInteropLoweringContract(dependency_contract) &&
      dependency_contract.deterministic &&
      cpp_summary.deterministic &&
      cpp_summary.ready_for_lowering_and_runtime &&
      preservation_summary.deterministic &&
      preservation_summary.runtime_import_artifact_ready &&
      preservation_summary.separate_compilation_preservation_ready;
  return contract;
}

static std::string SanitizeMetaprogrammingArtifactToken(const std::string &text) {
  std::string out;
  out.reserve(text.size());
  for (unsigned char ch : text) {
    if (std::isalnum(ch) != 0) {
      out.push_back(static_cast<char>(ch));
    } else {
      out.push_back('_');
    }
  }
  if (out.empty()) {
    out = "anonymous";
  }
  return out;
}

static std::string BuildMetaprogrammingRuntimeClassOwnerIdentity(
    const std::string &class_name) {
  return "class:" + class_name;
}

static std::string BuildMetaprogrammingRuntimeCategoryOwnerIdentity(
    const std::string &class_name, const std::string &category_name) {
  return "category:" + class_name + "(" + category_name + ")";
}

static std::string BuildMetaprogrammingCategoryOwnerName(const std::string &class_name,
                                                const std::string &category_name) {
  return class_name + "(" + category_name + ")";
}

static std::string BuildMetaprogrammingDerivedMethodFunctionSymbol(
    const std::string &implementation_name, const std::string &selector) {
  return "objc3_method_" + SanitizeMetaprogrammingArtifactToken(implementation_name) +
         "_instance_" + SanitizeMetaprogrammingArtifactToken(selector);
}

static std::string BuildMetaprogrammingMacroArtifactSymbol(
    const std::string &function_name, const std::string &macro_name) {
  return "objc3_metaprogramming_macro_artifact_" +
         SanitizeMetaprogrammingArtifactToken(function_name) + "_" +
         SanitizeMetaprogrammingArtifactToken(macro_name);
}

static std::string BuildMetaprogrammingPropertyBehaviorArtifactSymbol(
    const std::string &owner_kind, const std::string &owner_name,
    const std::string &property_name, const std::string &behavior_name) {
  return "objc3_metaprogramming_property_behavior_" +
         SanitizeMetaprogrammingArtifactToken(owner_kind) + "_" +
         SanitizeMetaprogrammingArtifactToken(owner_name) + "_" +
         SanitizeMetaprogrammingArtifactToken(property_name) + "_" +
         SanitizeMetaprogrammingArtifactToken(behavior_name);
}

static std::string NormalizeMetaprogrammingNamedStringToken(std::string token) {
  token.erase(std::remove_if(token.begin(), token.end(),
                             [](unsigned char ch) { return std::isspace(ch) != 0; }),
              token.end());
  if (token.size() >= 2u && token.front() == '"' && token.back() == '"') {
    token = token.substr(1u, token.size() - 2u);
  }
  return token;
}

static std::string NormalizeMetaprogrammingPropertyBehaviorNameForEmission(
    std::string token) {
  token = NormalizeMetaprogrammingNamedStringToken(std::move(token));
  std::transform(token.begin(), token.end(), token.begin(),
                 [](unsigned char ch) { return static_cast<char>(std::tolower(ch)); });
  if (token == "observed") {
    return "Observed";
  }
  if (token == "projected") {
    return "Projected";
  }
  return {};
}

static std::string NormalizeMetaprogrammingDeriveNameForEmission(std::string token) {
  token = NormalizeMetaprogrammingNamedStringToken(std::move(token));
  std::transform(token.begin(), token.end(), token.begin(),
                 [](unsigned char ch) { return static_cast<char>(std::tolower(ch)); });
  if (token == "equality" || token == "equatable") {
    return "Equality";
  }
  if (token == "hash") {
    return "Hash";
  }
  if (token == "debugdescription") {
    return "DebugDescription";
  }
  return {};
}

static bool IsSupportedMetaprogrammingMacroPackageNameForEmission(
    std::string package_name) {
  package_name = NormalizeMetaprogrammingNamedStringToken(std::move(package_name));
  std::transform(package_name.begin(), package_name.end(), package_name.begin(),
                 [](unsigned char ch) { return static_cast<char>(std::tolower(ch)); });
  return package_name == "std.metaprogramming" ||
         package_name == "std.metaprogramming.trace";
}

static bool IsSupportedMetaprogrammingMacroProvenanceNameForEmission(
    std::string provenance_name) {
  provenance_name = NormalizeMetaprogrammingNamedStringToken(std::move(provenance_name));
  std::transform(provenance_name.begin(), provenance_name.end(),
                 provenance_name.begin(),
                 [](unsigned char ch) { return static_cast<char>(std::tolower(ch)); });
  constexpr std::string_view prefix = "sha256:";
  if (provenance_name.size() < prefix.size() ||
      provenance_name.compare(0u, prefix.size(), prefix) != 0) {
    return false;
  }
  const std::string_view digest(provenance_name.c_str() + prefix.size(),
                                provenance_name.size() - prefix.size());
  return digest.size() >= 6u && digest.size() <= 64u &&
         std::all_of(digest.begin(), digest.end(), [](char c) {
           return (c >= '0' && c <= '9') || (c >= 'a' && c <= 'f');
         });
}

static bool IsMetaprogrammingPropertyBehaviorObjectLike(const Objc3PropertyDecl &property) {
  return property.id_spelling || property.instancetype_spelling ||
         property.object_pointer_type_spelling ||
         property.type == ValueType::ObjCId ||
         property.type == ValueType::ObjCClass ||
         property.type == ValueType::ObjCInstancetype ||
         property.type == ValueType::ObjCObjectPtr;
}

std::vector<Objc3IRMetaprogrammingDerivedMethodBundle> BuildMetaprogrammingDerivedMethodBundles(
    const Objc3Program &program) {
  std::unordered_map<std::string, const Objc3ImplementationDecl *> implementations;
  implementations.reserve(program.implementations.size());
  for (const auto &implementation_decl : program.implementations) {
    if (!implementation_decl.has_category) {
      implementations.emplace(implementation_decl.name, &implementation_decl);
    }
  }

  std::vector<Objc3IRMetaprogrammingDerivedMethodBundle> bundles;
  for (const auto &interface_decl : program.interfaces) {
    if (interface_decl.has_category) {
      continue;
    }
    const bool has_derive_marker =
        interface_decl.objc_derive_declared ||
        !interface_decl.objc_derive_name.empty();
    if (!has_derive_marker) {
      continue;
    }
    const auto impl_it = implementations.find(interface_decl.name);
    if (impl_it == implementations.end()) {
      continue;
    }
    const std::string derive_name =
        NormalizeMetaprogrammingDeriveNameForEmission(interface_decl.objc_derive_name);
    if (derive_name.empty()) {
      continue;
    }

    std::string selector;
    std::size_t parameter_count = 0u;
    if (derive_name == "Equality") {
      selector = "isEqual:";
      parameter_count = 1u;
    } else if (derive_name == "Hash") {
      selector = "hash";
    } else if (derive_name == "DebugDescription") {
      selector = "debugDescription";
    }
    const auto selector_exists = [&selector](const auto &methods) {
      return std::any_of(methods.begin(), methods.end(),
                         [&selector](const Objc3MethodDecl &method_decl) {
                           return !method_decl.is_class_method &&
                                  method_decl.selector == selector;
                         });
    };
    if (selector.empty() || selector_exists(interface_decl.methods) ||
        selector_exists(impl_it->second->methods)) {
      continue;
    }

    Objc3IRMetaprogrammingDerivedMethodBundle bundle;
    bundle.implementation_name = impl_it->second->name;
    bundle.declaration_owner_identity = impl_it->second->semantic_link_symbol;
    bundle.export_owner_identity =
        BuildMetaprogrammingRuntimeClassOwnerIdentity(interface_decl.name);
    bundle.derive_name = derive_name;
    bundle.selector = selector;
    bundle.emitted_symbol =
        BuildMetaprogrammingDerivedMethodFunctionSymbol(bundle.implementation_name,
                                               bundle.selector);
    bundle.parameter_count = parameter_count;
    bundle.line = interface_decl.line;
    bundle.column = interface_decl.column;
    bundles.push_back(std::move(bundle));
  }

  std::sort(bundles.begin(), bundles.end(),
            [](const Objc3IRMetaprogrammingDerivedMethodBundle &lhs,
               const Objc3IRMetaprogrammingDerivedMethodBundle &rhs) {
              return std::tie(lhs.implementation_name, lhs.selector, lhs.derive_name) <
                     std::tie(rhs.implementation_name, rhs.selector, rhs.derive_name);
            });
  return bundles;
}

std::vector<Objc3IRMetaprogrammingMacroArtifactBundle> BuildMetaprogrammingMacroArtifactBundles(
    const Objc3Program &program) {
  std::vector<Objc3IRMetaprogrammingMacroArtifactBundle> bundles;
  for (const auto &fn : program.functions) {
    const bool has_macro_marker =
        fn.objc_macro_declared || !fn.objc_macro_name.empty();
    const bool has_macro_package =
        fn.objc_macro_package_declared || !fn.objc_macro_package_name.empty();
    const bool has_macro_provenance =
        fn.objc_macro_provenance_declared ||
        !fn.objc_macro_provenance_name.empty();
    if (!has_macro_marker || !has_macro_package || !has_macro_provenance ||
        !fn.is_pure || fn.is_prototype || fn.async_declared ||
        fn.throws_declared ||
        !IsSupportedMetaprogrammingMacroPackageNameForEmission(fn.objc_macro_package_name) ||
        !IsSupportedMetaprogrammingMacroProvenanceNameForEmission(
            fn.objc_macro_provenance_name)) {
      continue;
    }
    Objc3IRMetaprogrammingMacroArtifactBundle bundle;
    bundle.function_name = fn.name;
    bundle.macro_name = fn.objc_macro_name;
    bundle.package_name = fn.objc_macro_package_name;
    bundle.provenance_name = fn.objc_macro_provenance_name;
    bundle.cache_key_name = fn.objc_macro_cache_key_name;
    bundle.sandbox_policy_name = fn.objc_macro_sandbox_name;
    bundle.emitted_symbol =
        BuildMetaprogrammingMacroArtifactSymbol(fn.name, fn.objc_macro_name);
    bundle.line = fn.line;
    bundle.column = fn.column;
    bundles.push_back(std::move(bundle));
  }
  std::sort(bundles.begin(), bundles.end(),
            [](const Objc3IRMetaprogrammingMacroArtifactBundle &lhs,
               const Objc3IRMetaprogrammingMacroArtifactBundle &rhs) {
              return std::tie(lhs.function_name, lhs.macro_name,
                              lhs.provenance_name, lhs.cache_key_name,
                              lhs.sandbox_policy_name) <
                     std::tie(rhs.function_name, rhs.macro_name,
                              rhs.provenance_name, rhs.cache_key_name,
                              rhs.sandbox_policy_name);
            });
  return bundles;
}

std::vector<Objc3IRMetaprogrammingPropertyBehaviorArtifactBundle>
BuildMetaprogrammingPropertyBehaviorArtifactBundles(const Objc3Program &program) {
  std::vector<Objc3IRMetaprogrammingPropertyBehaviorArtifactBundle> bundles;
  const auto append_properties =
      [&bundles](const auto &properties, const std::string &owner_kind,
                 const std::string &owner_name,
                 const std::string &declaration_owner_identity,
                 const std::string &export_owner_identity) {
        for (const auto &property : properties) {
          if (!property.property_behavior_declared) {
            continue;
          }
          const std::string behavior_name =
              NormalizeMetaprogrammingPropertyBehaviorNameForEmission(
                  property.property_behavior_name);
          if (behavior_name.empty() ||
              !IsMetaprogrammingPropertyBehaviorObjectLike(property)) {
            continue;
          }
          if (behavior_name == "Observed" &&
              (owner_kind == "protocol" || property.is_readonly ||
               !property.effective_setter_available)) {
            continue;
          }
          if (behavior_name == "Projected" &&
              (!property.is_readonly || property.has_setter ||
               property.effective_setter_available)) {
            continue;
          }

          Objc3IRMetaprogrammingPropertyBehaviorArtifactBundle bundle;
          bundle.owner_kind = owner_kind;
          bundle.owner_name = owner_name;
          bundle.declaration_owner_identity = declaration_owner_identity;
          bundle.export_owner_identity = export_owner_identity;
          bundle.property_name = property.name;
          bundle.behavior_name = behavior_name;
          bundle.binding_symbol = property.executable_synthesized_binding_symbol;
          if (bundle.binding_symbol.empty()) {
            bundle.binding_symbol = property.ivar_binding_symbol;
          }
          bundle.emitted_symbol = BuildMetaprogrammingPropertyBehaviorArtifactSymbol(
              owner_kind, owner_name, property.name, behavior_name);
          bundle.line = property.line;
          bundle.column = property.column;
          bundles.push_back(std::move(bundle));
        }
      };

  for (const auto &interface_decl : program.interfaces) {
    append_properties(
        interface_decl.properties,
        interface_decl.has_category ? "category-interface" : "class-interface",
        interface_decl.has_category
            ? BuildMetaprogrammingCategoryOwnerName(interface_decl.name,
                                           interface_decl.category_name)
            : interface_decl.name,
        interface_decl.semantic_link_symbol,
        interface_decl.has_category
            ? BuildMetaprogrammingRuntimeCategoryOwnerIdentity(interface_decl.name,
                                                      interface_decl.category_name)
            : BuildMetaprogrammingRuntimeClassOwnerIdentity(interface_decl.name));
  }
  for (const auto &protocol_decl : program.protocols) {
    append_properties(protocol_decl.properties, "protocol", protocol_decl.name,
