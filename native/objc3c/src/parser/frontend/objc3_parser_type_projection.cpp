#include "parser/frontend/objc3_parser_type_projection.h"

namespace objc3c::parse {

void CopyObjc3MethodReturnTypeFromFunctionDecl(const FunctionDecl &source,
                                               Objc3MethodDecl &target) {
  target.return_type = source.return_type;
  target.return_vector_spelling = source.return_vector_spelling;
  target.return_vector_base_spelling = source.return_vector_base_spelling;
  target.return_vector_lane_count = source.return_vector_lane_count;
  target.return_id_spelling = source.return_id_spelling;
  target.return_class_spelling = source.return_class_spelling;
  target.return_sel_spelling = source.return_sel_spelling;
  target.return_instancetype_spelling = source.return_instancetype_spelling;
  target.return_object_pointer_type_spelling = source.return_object_pointer_type_spelling;
  target.return_object_pointer_type_name = source.return_object_pointer_type_name;
  target.return_typecheck_family_symbol = source.return_typecheck_family_symbol;
  target.has_return_generic_suffix = source.has_return_generic_suffix;
  target.return_generic_suffix_terminated = source.return_generic_suffix_terminated;
  target.return_generic_suffix_text = source.return_generic_suffix_text;
  target.return_generic_line = source.return_generic_line;
  target.return_generic_column = source.return_generic_column;
  target.return_lightweight_generic_constraint_profile_is_normalized =
      source.return_lightweight_generic_constraint_profile_is_normalized;
  target.return_lightweight_generic_constraint_profile =
      source.return_lightweight_generic_constraint_profile;
  target.return_nullability_flow_profile_is_normalized =
      source.return_nullability_flow_profile_is_normalized;
  target.return_nullability_flow_profile =
      source.return_nullability_flow_profile;
  target.return_protocol_qualified_object_type_profile_is_normalized =
      source.return_protocol_qualified_object_type_profile_is_normalized;
  target.return_protocol_qualified_object_type_profile =
      source.return_protocol_qualified_object_type_profile;
  target.return_variance_bridge_cast_profile_is_normalized =
      source.return_variance_bridge_cast_profile_is_normalized;
  target.return_variance_bridge_cast_profile =
      source.return_variance_bridge_cast_profile;
  target.return_generic_metadata_abi_profile_is_normalized =
      source.return_generic_metadata_abi_profile_is_normalized;
  target.return_generic_metadata_abi_profile =
      source.return_generic_metadata_abi_profile;
  target.return_module_import_graph_profile_is_normalized =
      source.return_module_import_graph_profile_is_normalized;
  target.return_module_import_graph_profile =
      source.return_module_import_graph_profile;
  target.return_namespace_collision_shadowing_profile_is_normalized =
      source.return_namespace_collision_shadowing_profile_is_normalized;
  target.return_namespace_collision_shadowing_profile =
      source.return_namespace_collision_shadowing_profile;
  target.return_public_private_api_partition_profile_is_normalized =
      source.return_public_private_api_partition_profile_is_normalized;
  target.return_public_private_api_partition_profile =
      source.return_public_private_api_partition_profile;
  target.return_incremental_module_cache_invalidation_profile_is_normalized =
      source.return_incremental_module_cache_invalidation_profile_is_normalized;
  target.return_incremental_module_cache_invalidation_profile =
      source.return_incremental_module_cache_invalidation_profile;
  target.return_cross_module_conformance_profile_is_normalized =
      source.return_cross_module_conformance_profile_is_normalized;
  target.return_cross_module_conformance_profile =
      source.return_cross_module_conformance_profile;
  target.has_return_pointer_declarator = source.has_return_pointer_declarator;
  target.return_pointer_declarator_depth = source.return_pointer_declarator_depth;
  target.return_pointer_declarator_tokens = source.return_pointer_declarator_tokens;
  target.return_nullability_suffix_tokens = source.return_nullability_suffix_tokens;
  target.has_return_ownership_qualifier = source.has_return_ownership_qualifier;
  target.return_ownership_qualifier_spelling = source.return_ownership_qualifier_spelling;
  target.return_ownership_qualifier_symbol = source.return_ownership_qualifier_symbol;
  target.return_ownership_qualifier_tokens = source.return_ownership_qualifier_tokens;
  target.return_ownership_insert_retain = source.return_ownership_insert_retain;
  target.return_ownership_insert_release = source.return_ownership_insert_release;
  target.return_ownership_insert_autorelease = source.return_ownership_insert_autorelease;
  target.return_ownership_operation_profile = source.return_ownership_operation_profile;
  target.return_ownership_is_weak_reference = source.return_ownership_is_weak_reference;
  target.return_ownership_is_unowned_reference = source.return_ownership_is_unowned_reference;
  target.return_ownership_is_unowned_safe_reference = source.return_ownership_is_unowned_safe_reference;
  target.return_ownership_lifetime_profile = source.return_ownership_lifetime_profile;
  target.return_ownership_runtime_hook_profile = source.return_ownership_runtime_hook_profile;
  target.return_ownership_arc_diagnostic_candidate = source.return_ownership_arc_diagnostic_candidate;
  target.return_ownership_arc_fixit_available = source.return_ownership_arc_fixit_available;
  target.return_ownership_arc_diagnostic_profile = source.return_ownership_arc_diagnostic_profile;
  target.return_ownership_arc_fixit_hint = source.return_ownership_arc_fixit_hint;
  target.return_borrowed_pointer_qualified = source.return_borrowed_pointer_qualified;
  target.objc_returns_borrowed_declared = source.objc_returns_borrowed_declared;
  target.objc_returns_borrowed_owner_index = source.objc_returns_borrowed_owner_index;
  target.returns_borrowed_profile = source.returns_borrowed_profile;
  target.throws_declared = source.throws_declared;
  target.throws_declaration_profile_is_normalized =
      source.throws_declaration_profile_is_normalized;
  target.throws_declaration_profile = source.throws_declaration_profile;
  target.result_like_profile_is_normalized = source.result_like_profile_is_normalized;
  target.deterministic_result_like_lowering_handoff =
      source.deterministic_result_like_lowering_handoff;
  target.result_like_sites = source.result_like_sites;
  target.result_success_sites = source.result_success_sites;
  target.result_failure_sites = source.result_failure_sites;
  target.result_branch_sites = source.result_branch_sites;
  target.result_payload_sites = source.result_payload_sites;
  target.result_normalized_sites = source.result_normalized_sites;
  target.result_branch_merge_sites = source.result_branch_merge_sites;
  target.result_contract_violation_sites = source.result_contract_violation_sites;
  target.result_like_profile = source.result_like_profile;
  target.ns_error_bridging_profile_is_normalized = source.ns_error_bridging_profile_is_normalized;
  target.deterministic_ns_error_bridging_lowering_handoff =
      source.deterministic_ns_error_bridging_lowering_handoff;
  target.ns_error_bridging_sites = source.ns_error_bridging_sites;
  target.ns_error_parameter_sites = source.ns_error_parameter_sites;
  target.ns_error_out_parameter_sites = source.ns_error_out_parameter_sites;
  target.ns_error_bridge_path_sites = source.ns_error_bridge_path_sites;
  target.failable_call_sites = source.failable_call_sites;
  target.ns_error_bridging_normalized_sites = source.ns_error_bridging_normalized_sites;
  target.ns_error_bridge_boundary_sites = source.ns_error_bridge_boundary_sites;
  target.ns_error_bridging_contract_violation_sites = source.ns_error_bridging_contract_violation_sites;
  target.ns_error_bridging_profile = source.ns_error_bridging_profile;
  target.objc_nserror_declared = source.objc_nserror_declared;
  target.objc_status_code_declared = source.objc_status_code_declared;
  target.error_bridge_marker_profile_is_normalized =
      source.error_bridge_marker_profile_is_normalized;
  target.objc_nserror_attribute_sites = source.objc_nserror_attribute_sites;
  target.objc_status_code_attribute_sites = source.objc_status_code_attribute_sites;
  target.status_code_success_clause_sites = source.status_code_success_clause_sites;
  target.status_code_error_type_clause_sites =
      source.status_code_error_type_clause_sites;
  target.status_code_mapping_clause_sites = source.status_code_mapping_clause_sites;
  target.error_bridge_marker_contract_violation_sites =
      source.error_bridge_marker_contract_violation_sites;
  target.objc_status_code_success_literal =
      source.objc_status_code_success_literal;
  target.objc_status_code_error_type_spelling =
      source.objc_status_code_error_type_spelling;
  target.objc_status_code_mapping_symbol =
      source.objc_status_code_mapping_symbol;
  target.error_bridge_marker_profile = source.error_bridge_marker_profile;
  target.unwind_cleanup_profile_is_normalized = source.unwind_cleanup_profile_is_normalized;
  target.deterministic_unwind_cleanup_handoff =
      source.deterministic_unwind_cleanup_handoff;
  target.unwind_cleanup_sites = source.unwind_cleanup_sites;
  target.exceptional_exit_sites = source.exceptional_exit_sites;
  target.cleanup_action_sites = source.cleanup_action_sites;
  target.cleanup_scope_sites = source.cleanup_scope_sites;
  target.cleanup_resume_sites = source.cleanup_resume_sites;
  target.unwind_cleanup_normalized_sites = source.unwind_cleanup_normalized_sites;
  target.unwind_cleanup_fail_closed_sites = source.unwind_cleanup_fail_closed_sites;
  target.unwind_cleanup_contract_violation_sites =
      source.unwind_cleanup_contract_violation_sites;
  target.unwind_cleanup_profile = source.unwind_cleanup_profile;
  target.error_diagnostics_recovery_profile_is_normalized =
      source.error_diagnostics_recovery_profile_is_normalized;
  target.deterministic_error_diagnostics_recovery_handoff =
      source.deterministic_error_diagnostics_recovery_handoff;
  target.error_diagnostics_recovery_sites =
      source.error_diagnostics_recovery_sites;
  target.diagnostic_emit_sites = source.diagnostic_emit_sites;
  target.recovery_anchor_sites = source.recovery_anchor_sites;
  target.recovery_boundary_sites = source.recovery_boundary_sites;
  target.fail_closed_diagnostic_sites = source.fail_closed_diagnostic_sites;
  target.error_diagnostics_recovery_normalized_sites =
      source.error_diagnostics_recovery_normalized_sites;
  target.error_diagnostics_recovery_gate_blocked_sites =
      source.error_diagnostics_recovery_gate_blocked_sites;
  target.error_diagnostics_recovery_contract_violation_sites =
      source.error_diagnostics_recovery_contract_violation_sites;
  target.error_diagnostics_recovery_profile =
      source.error_diagnostics_recovery_profile;
  target.async_continuation_profile_is_normalized =
      source.async_continuation_profile_is_normalized;
  target.deterministic_async_continuation_handoff =
      source.deterministic_async_continuation_handoff;
  target.async_continuation_sites = source.async_continuation_sites;
  target.async_keyword_sites = source.async_keyword_sites;
  target.async_function_sites = source.async_function_sites;
  target.continuation_allocation_sites = source.continuation_allocation_sites;
  target.continuation_resume_sites = source.continuation_resume_sites;
  target.continuation_suspend_sites = source.continuation_suspend_sites;
  target.async_state_machine_sites = source.async_state_machine_sites;
  target.async_continuation_normalized_sites =
      source.async_continuation_normalized_sites;
  target.async_continuation_gate_blocked_sites =
      source.async_continuation_gate_blocked_sites;
  target.async_continuation_contract_violation_sites =
      source.async_continuation_contract_violation_sites;
  target.async_continuation_profile = source.async_continuation_profile;
  target.await_suspension_profile_is_normalized =
      source.await_suspension_profile_is_normalized;
  target.deterministic_await_suspension_handoff =
      source.deterministic_await_suspension_handoff;
  target.await_suspension_sites = source.await_suspension_sites;
  target.await_keyword_sites = source.await_keyword_sites;
  target.await_suspension_point_sites = source.await_suspension_point_sites;
  target.await_resume_sites = source.await_resume_sites;
  target.await_state_machine_sites = source.await_state_machine_sites;
  target.await_continuation_sites = source.await_continuation_sites;
  target.await_suspension_normalized_sites =
      source.await_suspension_normalized_sites;
  target.await_suspension_gate_blocked_sites =
      source.await_suspension_gate_blocked_sites;
  target.await_suspension_contract_violation_sites =
      source.await_suspension_contract_violation_sites;
  target.await_suspension_profile = source.await_suspension_profile;
  target.actor_isolation_sendability_profile_is_normalized =
      source.actor_isolation_sendability_profile_is_normalized;
  target.deterministic_actor_isolation_sendability_handoff =
      source.deterministic_actor_isolation_sendability_handoff;
  target.actor_isolation_sendability_sites =
      source.actor_isolation_sendability_sites;
  target.actor_isolation_decl_sites = source.actor_isolation_decl_sites;
  target.actor_hop_sites = source.actor_hop_sites;
  target.sendable_annotation_sites = source.sendable_annotation_sites;
  target.non_sendable_crossing_sites = source.non_sendable_crossing_sites;
  target.isolation_boundary_sites = source.isolation_boundary_sites;
  target.actor_isolation_sendability_normalized_sites =
      source.actor_isolation_sendability_normalized_sites;
  target.actor_isolation_sendability_gate_blocked_sites =
      source.actor_isolation_sendability_gate_blocked_sites;
  target.actor_isolation_sendability_contract_violation_sites =
      source.actor_isolation_sendability_contract_violation_sites;
  target.actor_isolation_sendability_profile =
      source.actor_isolation_sendability_profile;
  target.task_runtime_cancellation_profile_is_normalized =
      source.task_runtime_cancellation_profile_is_normalized;
  target.deterministic_task_runtime_cancellation_handoff =
      source.deterministic_task_runtime_cancellation_handoff;
  target.task_runtime_interop_sites = source.task_runtime_interop_sites;
  target.runtime_hook_sites = source.runtime_hook_sites;
  target.cancellation_check_sites = source.cancellation_check_sites;
  target.cancellation_handler_sites = source.cancellation_handler_sites;
  target.suspension_point_sites = source.suspension_point_sites;
  target.cancellation_propagation_sites =
      source.cancellation_propagation_sites;
  target.task_runtime_normalized_sites = source.task_runtime_normalized_sites;
  target.task_runtime_gate_blocked_sites = source.task_runtime_gate_blocked_sites;
  target.task_runtime_contract_violation_sites =
      source.task_runtime_contract_violation_sites;
  target.task_runtime_cancellation_normalized_sites =
      source.task_runtime_cancellation_normalized_sites;
  target.task_runtime_cancellation_gate_blocked_sites =
      source.task_runtime_cancellation_gate_blocked_sites;
  target.task_runtime_cancellation_contract_violation_sites =
      source.task_runtime_cancellation_contract_violation_sites;
  target.task_runtime_cancellation_profile =
      source.task_runtime_cancellation_profile;
  target.concurrency_replay_race_guard_profile_is_normalized =
      source.concurrency_replay_race_guard_profile_is_normalized;
  target.deterministic_concurrency_replay_race_guard_handoff =
      source.deterministic_concurrency_replay_race_guard_handoff;
  target.concurrency_replay_race_guard_sites =
      source.concurrency_replay_race_guard_sites;
  target.concurrency_replay_sites = source.concurrency_replay_sites;
  target.replay_proof_sites = source.replay_proof_sites;
  target.race_guard_sites = source.race_guard_sites;
  target.task_handoff_sites = source.task_handoff_sites;
  target.actor_isolation_sites = source.actor_isolation_sites;
  target.deterministic_schedule_sites = source.deterministic_schedule_sites;
  target.concurrency_replay_guard_blocked_sites =
      source.concurrency_replay_guard_blocked_sites;
  target.concurrency_replay_contract_violation_sites =
      source.concurrency_replay_contract_violation_sites;
  target.concurrency_replay_race_guard_profile =
      source.concurrency_replay_race_guard_profile;
  target.unsafe_pointer_extension_profile_is_normalized =
      source.unsafe_pointer_extension_profile_is_normalized;
  target.deterministic_unsafe_pointer_extension_handoff =
      source.deterministic_unsafe_pointer_extension_handoff;
  target.unsafe_pointer_extension_sites = source.unsafe_pointer_extension_sites;
  target.unsafe_keyword_sites = source.unsafe_keyword_sites;
  target.pointer_arithmetic_sites = source.pointer_arithmetic_sites;
  target.raw_pointer_type_sites = source.raw_pointer_type_sites;
  target.unsafe_operation_sites = source.unsafe_operation_sites;
  target.unsafe_pointer_extension_normalized_sites =
      source.unsafe_pointer_extension_normalized_sites;
  target.unsafe_pointer_extension_gate_blocked_sites =
      source.unsafe_pointer_extension_gate_blocked_sites;
  target.unsafe_pointer_extension_contract_violation_sites =
      source.unsafe_pointer_extension_contract_violation_sites;
  target.unsafe_pointer_extension_profile = source.unsafe_pointer_extension_profile;
  target.inline_asm_intrinsic_governance_profile_is_normalized =
      source.inline_asm_intrinsic_governance_profile_is_normalized;
  target.deterministic_inline_asm_intrinsic_governance_handoff =
      source.deterministic_inline_asm_intrinsic_governance_handoff;
  target.inline_asm_intrinsic_sites = source.inline_asm_intrinsic_sites;
  target.inline_asm_sites = source.inline_asm_sites;
  target.intrinsic_sites = source.intrinsic_sites;
  target.governed_intrinsic_sites = source.governed_intrinsic_sites;
  target.privileged_intrinsic_sites = source.privileged_intrinsic_sites;
  target.inline_asm_intrinsic_normalized_sites =
      source.inline_asm_intrinsic_normalized_sites;
  target.inline_asm_intrinsic_gate_blocked_sites =
      source.inline_asm_intrinsic_gate_blocked_sites;
  target.inline_asm_intrinsic_contract_violation_sites =
      source.inline_asm_intrinsic_contract_violation_sites;
  target.inline_asm_intrinsic_governance_profile =
      source.inline_asm_intrinsic_governance_profile;
}

void CopyObjc3PropertyTypeFromParam(const FuncParam &source,
                                    Objc3PropertyDecl &target) {
  target.type = source.type;
  target.vector_spelling = source.vector_spelling;
  target.vector_base_spelling = source.vector_base_spelling;
  target.vector_lane_count = source.vector_lane_count;
  target.id_spelling = source.id_spelling;
  target.class_spelling = source.class_spelling;
  target.sel_spelling = source.sel_spelling;
  target.instancetype_spelling = source.instancetype_spelling;
  target.object_pointer_type_spelling = source.object_pointer_type_spelling;
  target.object_pointer_type_name = source.object_pointer_type_name;
  target.typecheck_family_symbol = source.typecheck_family_symbol;
  target.has_generic_suffix = source.has_generic_suffix;
  target.generic_suffix_terminated = source.generic_suffix_terminated;
  target.generic_suffix_text = source.generic_suffix_text;
  target.generic_line = source.generic_line;
  target.generic_column = source.generic_column;
  target.lightweight_generic_constraint_profile_is_normalized =
      source.lightweight_generic_constraint_profile_is_normalized;
  target.lightweight_generic_constraint_profile =
      source.lightweight_generic_constraint_profile;
  target.nullability_flow_profile_is_normalized =
      source.nullability_flow_profile_is_normalized;
  target.nullability_flow_profile =
      source.nullability_flow_profile;
  target.protocol_qualified_object_type_profile_is_normalized =
      source.protocol_qualified_object_type_profile_is_normalized;
  target.protocol_qualified_object_type_profile =
      source.protocol_qualified_object_type_profile;
  target.variance_bridge_cast_profile_is_normalized =
      source.variance_bridge_cast_profile_is_normalized;
  target.variance_bridge_cast_profile =
      source.variance_bridge_cast_profile;
  target.generic_metadata_abi_profile_is_normalized =
      source.generic_metadata_abi_profile_is_normalized;
  target.generic_metadata_abi_profile =
      source.generic_metadata_abi_profile;
  target.module_import_graph_profile_is_normalized =
      source.module_import_graph_profile_is_normalized;
  target.module_import_graph_profile =
      source.module_import_graph_profile;
  target.namespace_collision_shadowing_profile_is_normalized =
      source.namespace_collision_shadowing_profile_is_normalized;
  target.namespace_collision_shadowing_profile =
      source.namespace_collision_shadowing_profile;
  target.public_private_api_partition_profile_is_normalized =
      source.public_private_api_partition_profile_is_normalized;
  target.public_private_api_partition_profile =
      source.public_private_api_partition_profile;
  target.incremental_module_cache_invalidation_profile_is_normalized =
      source.incremental_module_cache_invalidation_profile_is_normalized;
  target.incremental_module_cache_invalidation_profile =
      source.incremental_module_cache_invalidation_profile;
  target.cross_module_conformance_profile_is_normalized =
      source.cross_module_conformance_profile_is_normalized;
  target.cross_module_conformance_profile =
      source.cross_module_conformance_profile;
  target.has_pointer_declarator = source.has_pointer_declarator;
  target.pointer_declarator_depth = source.pointer_declarator_depth;
  target.pointer_declarator_tokens = source.pointer_declarator_tokens;
  target.nullability_suffix_tokens = source.nullability_suffix_tokens;
  target.has_ownership_qualifier = source.has_ownership_qualifier;
  target.ownership_qualifier_spelling = source.ownership_qualifier_spelling;
  target.ownership_qualifier_symbol = source.ownership_qualifier_symbol;
  target.ownership_qualifier_tokens = source.ownership_qualifier_tokens;
  target.ownership_insert_retain = source.ownership_insert_retain;
  target.ownership_insert_release = source.ownership_insert_release;
  target.ownership_insert_autorelease = source.ownership_insert_autorelease;
  target.ownership_operation_profile = source.ownership_operation_profile;
  target.ownership_is_weak_reference = source.ownership_is_weak_reference;
  target.ownership_is_unowned_reference = source.ownership_is_unowned_reference;
  target.ownership_is_unowned_safe_reference = source.ownership_is_unowned_safe_reference;
  target.ownership_lifetime_profile = source.ownership_lifetime_profile;
  target.ownership_runtime_hook_profile = source.ownership_runtime_hook_profile;
  target.ownership_arc_diagnostic_candidate = source.ownership_arc_diagnostic_candidate;
  target.ownership_arc_fixit_available = source.ownership_arc_fixit_available;
  target.ownership_arc_diagnostic_profile = source.ownership_arc_diagnostic_profile;
  target.ownership_arc_fixit_hint = source.ownership_arc_fixit_hint;
}

}  // namespace objc3c::parse
