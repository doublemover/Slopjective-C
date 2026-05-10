#include "sema/objc3_sema_pass_manager_contract_flow.h"

Objc3SemaControlBindingParityValidationReadinessRecord
BuildObjc3SemaControlBindingParityValidationReadinessRecord(
    const Objc3SemaPassManagerInput &input,
    const Objc3SemaParityContractSurface &surface,
    bool deterministic_unwind_cleanup_handoff,
    bool deterministic_async_continuation_handoff,
    bool deterministic_symbol_graph_scope_resolution_handoff,
    bool deterministic_method_lookup_override_conflict_handoff,
    bool deterministic_property_synthesis_ivar_binding_handoff,
    bool deterministic_id_class_sel_object_pointer_type_checking_handoff) {
  Objc3SemaControlBindingParityValidationReadinessRecord record;
  record.stage_input_owner = input.stage_input_owner;
  record.typed_semantic_handoff_owner = input.typed_semantic_handoff_owner;
  record.owner_model = input.owner_model;
  record.strict_no_retired_route = input.strict_no_retired_route;
  record.strict_no_compatibility = input.strict_no_compatibility;
  record.unwind_cleanup_ready =
      deterministic_unwind_cleanup_handoff &&
      surface.unwind_cleanup_summary.unwind_cleanup_sites ==
          surface.unwind_cleanup_sites_total &&
      surface.unwind_cleanup_summary.exceptional_exit_sites ==
          surface.unwind_cleanup_exceptional_exit_sites_total &&
      surface.unwind_cleanup_summary.cleanup_action_sites ==
          surface.unwind_cleanup_action_sites_total &&
      surface.unwind_cleanup_summary.cleanup_scope_sites ==
          surface.unwind_cleanup_scope_sites_total &&
      surface.unwind_cleanup_summary.cleanup_resume_sites ==
          surface.unwind_cleanup_resume_sites_total &&
      surface.unwind_cleanup_summary.normalized_sites ==
          surface.unwind_cleanup_normalized_sites_total &&
      surface.unwind_cleanup_summary.fail_closed_sites ==
          surface.unwind_cleanup_fail_closed_sites_total &&
      surface.unwind_cleanup_summary.contract_violation_sites ==
          surface.unwind_cleanup_contract_violation_sites_total &&
      surface.unwind_cleanup_summary.exceptional_exit_sites <=
          surface.unwind_cleanup_summary.unwind_cleanup_sites &&
      surface.unwind_cleanup_summary.cleanup_action_sites <=
          surface.unwind_cleanup_summary.unwind_cleanup_sites &&
      surface.unwind_cleanup_summary.cleanup_scope_sites <=
          surface.unwind_cleanup_summary.unwind_cleanup_sites &&
      surface.unwind_cleanup_summary.cleanup_resume_sites <=
          surface.unwind_cleanup_summary.unwind_cleanup_sites &&
      surface.unwind_cleanup_summary.normalized_sites <=
          surface.unwind_cleanup_summary.unwind_cleanup_sites &&
      surface.unwind_cleanup_summary.fail_closed_sites <=
          surface.unwind_cleanup_summary.unwind_cleanup_sites &&
      surface.unwind_cleanup_summary.contract_violation_sites <=
          surface.unwind_cleanup_summary.unwind_cleanup_sites &&
      surface.unwind_cleanup_summary.normalized_sites +
              surface.unwind_cleanup_summary.fail_closed_sites ==
          surface.unwind_cleanup_summary.unwind_cleanup_sites &&
      surface.unwind_cleanup_summary.deterministic;
  record.async_continuation_ready =
      deterministic_async_continuation_handoff &&
      surface.async_continuation_summary.async_continuation_sites ==
          surface.async_continuation_sites_total &&
      surface.async_continuation_summary.async_keyword_sites ==
          surface.async_continuation_async_keyword_sites_total &&
      surface.async_continuation_summary.async_function_sites ==
          surface.async_continuation_async_function_sites_total &&
      surface.async_continuation_summary.continuation_allocation_sites ==
          surface.async_continuation_allocation_sites_total &&
      surface.async_continuation_summary.continuation_resume_sites ==
          surface.async_continuation_resume_sites_total &&
      surface.async_continuation_summary.continuation_suspend_sites ==
          surface.async_continuation_suspend_sites_total &&
      surface.async_continuation_summary.async_state_machine_sites ==
          surface.async_continuation_state_machine_sites_total &&
      surface.async_continuation_summary.normalized_sites ==
          surface.async_continuation_normalized_sites_total &&
      surface.async_continuation_summary.gate_blocked_sites ==
          surface.async_continuation_gate_blocked_sites_total &&
      surface.async_continuation_summary.contract_violation_sites ==
          surface.async_continuation_contract_violation_sites_total &&
      surface.async_continuation_summary.async_keyword_sites <=
          surface.async_continuation_summary.async_continuation_sites &&
      surface.async_continuation_summary.async_function_sites <=
          surface.async_continuation_summary.async_continuation_sites &&
      surface.async_continuation_summary.continuation_allocation_sites <=
          surface.async_continuation_summary.async_continuation_sites &&
      surface.async_continuation_summary.continuation_resume_sites <=
          surface.async_continuation_summary.async_continuation_sites &&
      surface.async_continuation_summary.continuation_suspend_sites <=
          surface.async_continuation_summary.async_continuation_sites &&
      surface.async_continuation_summary.async_state_machine_sites <=
          surface.async_continuation_summary.async_continuation_sites &&
      surface.async_continuation_summary.normalized_sites <=
          surface.async_continuation_summary.async_continuation_sites &&
      surface.async_continuation_summary.gate_blocked_sites <=
          surface.async_continuation_summary.async_continuation_sites &&
      surface.async_continuation_summary.contract_violation_sites <=
          surface.async_continuation_summary.async_continuation_sites &&
      surface.async_continuation_summary.normalized_sites +
              surface.async_continuation_summary.gate_blocked_sites ==
          surface.async_continuation_summary.async_continuation_sites &&
      surface.async_continuation_summary.deterministic;
  record.symbol_graph_scope_resolution_ready =
      deterministic_symbol_graph_scope_resolution_handoff &&
      surface.symbol_graph_scope_resolution_summary.global_symbol_nodes ==
          surface.symbol_graph_global_symbol_nodes_total &&
      surface.symbol_graph_scope_resolution_summary.function_symbol_nodes ==
          surface.symbol_graph_function_symbol_nodes_total &&
      surface.symbol_graph_scope_resolution_summary.interface_symbol_nodes ==
          surface.symbol_graph_interface_symbol_nodes_total &&
      surface.symbol_graph_scope_resolution_summary.implementation_symbol_nodes ==
          surface.symbol_graph_implementation_symbol_nodes_total &&
      surface.symbol_graph_scope_resolution_summary
              .interface_property_symbol_nodes ==
          surface.symbol_graph_interface_property_symbol_nodes_total &&
      surface.symbol_graph_scope_resolution_summary
              .implementation_property_symbol_nodes ==
          surface.symbol_graph_implementation_property_symbol_nodes_total &&
      surface.symbol_graph_scope_resolution_summary.interface_method_symbol_nodes ==
          surface.symbol_graph_interface_method_symbol_nodes_total &&
      surface.symbol_graph_scope_resolution_summary
              .implementation_method_symbol_nodes ==
          surface.symbol_graph_implementation_method_symbol_nodes_total &&
      surface.symbol_graph_scope_resolution_summary.top_level_scope_symbols ==
          surface.symbol_graph_top_level_scope_symbols_total &&
      surface.symbol_graph_scope_resolution_summary.nested_scope_symbols ==
          surface.symbol_graph_nested_scope_symbols_total &&
      surface.symbol_graph_scope_resolution_summary.scope_frames_total ==
          surface.symbol_graph_scope_frames_total &&
      surface.symbol_graph_scope_resolution_summary
              .implementation_interface_resolution_sites ==
          surface
              .symbol_graph_implementation_interface_resolution_sites_total &&
      surface.symbol_graph_scope_resolution_summary
              .implementation_interface_resolution_hits ==
          surface
              .symbol_graph_implementation_interface_resolution_hits_total &&
      surface.symbol_graph_scope_resolution_summary
              .implementation_interface_resolution_misses ==
          surface
              .symbol_graph_implementation_interface_resolution_misses_total &&
      surface.symbol_graph_scope_resolution_summary.method_resolution_sites ==
          surface.symbol_graph_method_resolution_sites_total &&
      surface.symbol_graph_scope_resolution_summary.method_resolution_hits ==
          surface.symbol_graph_method_resolution_hits_total &&
      surface.symbol_graph_scope_resolution_summary.method_resolution_misses ==
          surface.symbol_graph_method_resolution_misses_total &&
      surface.symbol_graph_scope_resolution_summary.symbol_nodes_total() ==
          surface.symbol_graph_scope_resolution_summary
              .top_level_scope_symbols +
              surface.symbol_graph_scope_resolution_summary
                  .nested_scope_symbols &&
      surface.symbol_graph_scope_resolution_summary
              .implementation_interface_resolution_hits <=
          surface.symbol_graph_scope_resolution_summary
              .implementation_interface_resolution_sites &&
      surface.symbol_graph_scope_resolution_summary
              .implementation_interface_resolution_hits +
              surface.symbol_graph_scope_resolution_summary
                  .implementation_interface_resolution_misses ==
          surface.symbol_graph_scope_resolution_summary
              .implementation_interface_resolution_sites &&
      surface.symbol_graph_scope_resolution_summary.method_resolution_hits <=
          surface.symbol_graph_scope_resolution_summary.method_resolution_sites &&
      surface.symbol_graph_scope_resolution_summary.method_resolution_hits +
              surface.symbol_graph_scope_resolution_summary
                  .method_resolution_misses ==
          surface.symbol_graph_scope_resolution_summary.method_resolution_sites &&
      surface.symbol_graph_scope_resolution_summary.resolution_hits_total() <=
          surface.symbol_graph_scope_resolution_summary
              .resolution_sites_total() &&
      surface.symbol_graph_scope_resolution_summary.resolution_hits_total() +
              surface.symbol_graph_scope_resolution_summary
                  .resolution_misses_total() ==
          surface.symbol_graph_scope_resolution_summary
              .resolution_sites_total() &&
      surface.symbol_graph_scope_resolution_summary.deterministic;
  record.method_lookup_override_conflict_ready =
      deterministic_method_lookup_override_conflict_handoff &&
      surface.method_lookup_override_conflict_summary.method_lookup_sites ==
          surface.method_lookup_override_conflict_lookup_sites_total &&
      surface.method_lookup_override_conflict_summary.method_lookup_hits ==
          surface.method_lookup_override_conflict_lookup_hits_total &&
      surface.method_lookup_override_conflict_summary.method_lookup_misses ==
          surface.method_lookup_override_conflict_lookup_misses_total &&
      surface.method_lookup_override_conflict_summary.override_lookup_sites ==
          surface.method_lookup_override_conflict_override_sites_total &&
      surface.method_lookup_override_conflict_summary.override_lookup_hits ==
          surface.method_lookup_override_conflict_override_hits_total &&
      surface.method_lookup_override_conflict_summary.override_lookup_misses ==
          surface.method_lookup_override_conflict_override_misses_total &&
      surface.method_lookup_override_conflict_summary.override_conflicts ==
          surface.method_lookup_override_conflict_override_conflicts_total &&
      surface.method_lookup_override_conflict_summary
              .unresolved_base_interfaces ==
          surface
              .method_lookup_override_conflict_unresolved_base_interfaces_total &&
      surface.method_lookup_override_conflict_summary.method_lookup_hits <=
          surface.method_lookup_override_conflict_summary.method_lookup_sites &&
      surface.method_lookup_override_conflict_summary.method_lookup_hits +
              surface.method_lookup_override_conflict_summary
                  .method_lookup_misses ==
          surface.method_lookup_override_conflict_summary.method_lookup_sites &&
      surface.method_lookup_override_conflict_summary.override_lookup_hits <=
          surface.method_lookup_override_conflict_summary.override_lookup_sites &&
      surface.method_lookup_override_conflict_summary.override_lookup_hits +
              surface.method_lookup_override_conflict_summary
                  .override_lookup_misses ==
          surface.method_lookup_override_conflict_summary.override_lookup_sites &&
      surface.method_lookup_override_conflict_summary.override_conflicts <=
          surface.method_lookup_override_conflict_summary.override_lookup_hits &&
      surface.method_lookup_override_conflict_summary.deterministic;
  record.property_synthesis_ivar_binding_ready =
      deterministic_property_synthesis_ivar_binding_handoff &&
      surface.property_synthesis_ivar_binding_summary.property_synthesis_sites ==
          surface
              .property_synthesis_ivar_binding_property_synthesis_sites_total &&
      surface.property_synthesis_ivar_binding_summary
              .property_synthesis_explicit_ivar_bindings ==
          surface
              .property_synthesis_ivar_binding_explicit_ivar_bindings_total &&
      surface.property_synthesis_ivar_binding_summary
              .property_synthesis_default_ivar_bindings ==
          surface
              .property_synthesis_ivar_binding_default_ivar_bindings_total &&
      surface.property_synthesis_ivar_binding_summary.ivar_binding_sites ==
          surface.property_synthesis_ivar_binding_ivar_binding_sites_total &&
      surface.property_synthesis_ivar_binding_summary.ivar_binding_resolved ==
          surface.property_synthesis_ivar_binding_ivar_binding_resolved_total &&
      surface.property_synthesis_ivar_binding_summary.ivar_binding_missing ==
          surface.property_synthesis_ivar_binding_ivar_binding_missing_total &&
      surface.property_synthesis_ivar_binding_summary.ivar_binding_conflicts ==
          surface.property_synthesis_ivar_binding_ivar_binding_conflicts_total &&
      surface.property_synthesis_ivar_binding_summary
              .property_synthesis_explicit_ivar_bindings +
              surface.property_synthesis_ivar_binding_summary
                  .property_synthesis_default_ivar_bindings ==
          surface.property_synthesis_ivar_binding_summary
              .property_synthesis_sites &&
      surface.property_synthesis_ivar_binding_summary.ivar_binding_sites ==
          surface.property_synthesis_ivar_binding_summary
              .property_synthesis_sites &&
      surface.property_synthesis_ivar_binding_summary.ivar_binding_resolved +
              surface.property_synthesis_ivar_binding_summary
                  .ivar_binding_missing +
              surface.property_synthesis_ivar_binding_summary
                  .ivar_binding_conflicts ==
          surface.property_synthesis_ivar_binding_summary
              .ivar_binding_sites &&
      surface.property_synthesis_ivar_binding_summary.deterministic;
  record.id_class_sel_object_pointer_type_checking_ready =
      deterministic_id_class_sel_object_pointer_type_checking_handoff &&
      surface.id_class_sel_object_pointer_type_checking_summary
              .param_type_sites ==
          surface.id_class_sel_object_pointer_param_type_sites_total &&
      surface.id_class_sel_object_pointer_type_checking_summary
              .param_id_spelling_sites ==
          surface.id_class_sel_object_pointer_param_id_spelling_sites_total &&
      surface.id_class_sel_object_pointer_type_checking_summary
              .param_class_spelling_sites ==
          surface.id_class_sel_object_pointer_param_class_spelling_sites_total &&
      surface.id_class_sel_object_pointer_type_checking_summary
              .param_sel_spelling_sites ==
          surface.id_class_sel_object_pointer_param_sel_spelling_sites_total &&
      surface.id_class_sel_object_pointer_type_checking_summary
              .param_instancetype_spelling_sites ==
          surface
              .id_class_sel_object_pointer_param_instancetype_spelling_sites_total &&
      surface.id_class_sel_object_pointer_type_checking_summary
              .param_object_pointer_type_sites ==
          surface
              .id_class_sel_object_pointer_param_object_pointer_type_sites_total &&
      surface.id_class_sel_object_pointer_type_checking_summary
              .return_type_sites ==
          surface.id_class_sel_object_pointer_return_type_sites_total &&
      surface.id_class_sel_object_pointer_type_checking_summary
              .return_id_spelling_sites ==
          surface.id_class_sel_object_pointer_return_id_spelling_sites_total &&
      surface.id_class_sel_object_pointer_type_checking_summary
              .return_class_spelling_sites ==
          surface
              .id_class_sel_object_pointer_return_class_spelling_sites_total &&
      surface.id_class_sel_object_pointer_type_checking_summary
              .return_sel_spelling_sites ==
          surface.id_class_sel_object_pointer_return_sel_spelling_sites_total &&
      surface.id_class_sel_object_pointer_type_checking_summary
              .return_instancetype_spelling_sites ==
          surface
              .id_class_sel_object_pointer_return_instancetype_spelling_sites_total &&
      surface.id_class_sel_object_pointer_type_checking_summary
              .return_object_pointer_type_sites ==
          surface
              .id_class_sel_object_pointer_return_object_pointer_type_sites_total &&
      surface.id_class_sel_object_pointer_type_checking_summary
              .property_type_sites ==
          surface.id_class_sel_object_pointer_property_type_sites_total &&
      surface.id_class_sel_object_pointer_type_checking_summary
              .property_id_spelling_sites ==
          surface.id_class_sel_object_pointer_property_id_spelling_sites_total &&
      surface.id_class_sel_object_pointer_type_checking_summary
              .property_class_spelling_sites ==
          surface
              .id_class_sel_object_pointer_property_class_spelling_sites_total &&
      surface.id_class_sel_object_pointer_type_checking_summary
              .property_sel_spelling_sites ==
          surface.id_class_sel_object_pointer_property_sel_spelling_sites_total &&
      surface.id_class_sel_object_pointer_type_checking_summary
              .property_instancetype_spelling_sites ==
          surface
              .id_class_sel_object_pointer_property_instancetype_spelling_sites_total &&
      surface.id_class_sel_object_pointer_type_checking_summary
              .property_object_pointer_type_sites ==
          surface
              .id_class_sel_object_pointer_property_object_pointer_type_sites_total &&
      surface.id_class_sel_object_pointer_type_checking_summary
              .param_id_spelling_sites +
              surface.id_class_sel_object_pointer_type_checking_summary
                  .param_class_spelling_sites +
              surface.id_class_sel_object_pointer_type_checking_summary
                  .param_sel_spelling_sites +
              surface.id_class_sel_object_pointer_type_checking_summary
                  .param_instancetype_spelling_sites +
              surface.id_class_sel_object_pointer_type_checking_summary
                  .param_object_pointer_type_sites <=
          surface.id_class_sel_object_pointer_type_checking_summary
              .param_type_sites &&
      surface.id_class_sel_object_pointer_type_checking_summary
              .return_id_spelling_sites +
              surface.id_class_sel_object_pointer_type_checking_summary
                  .return_class_spelling_sites +
              surface.id_class_sel_object_pointer_type_checking_summary
                  .return_sel_spelling_sites +
              surface.id_class_sel_object_pointer_type_checking_summary
                  .return_instancetype_spelling_sites +
              surface.id_class_sel_object_pointer_type_checking_summary
                  .return_object_pointer_type_sites <=
          surface.id_class_sel_object_pointer_type_checking_summary
              .return_type_sites &&
      surface.id_class_sel_object_pointer_type_checking_summary
              .property_id_spelling_sites +
              surface.id_class_sel_object_pointer_type_checking_summary
                  .property_class_spelling_sites +
              surface.id_class_sel_object_pointer_type_checking_summary
                  .property_sel_spelling_sites +
              surface.id_class_sel_object_pointer_type_checking_summary
                  .property_instancetype_spelling_sites +
              surface.id_class_sel_object_pointer_type_checking_summary
                  .property_object_pointer_type_sites <=
          surface.id_class_sel_object_pointer_type_checking_summary
              .property_type_sites &&
      surface.id_class_sel_object_pointer_type_checking_summary.deterministic;
  record.passed_validation_count =
      Objc3SemaEvidenceCount(record.unwind_cleanup_ready) +
      Objc3SemaEvidenceCount(record.async_continuation_ready) +
      Objc3SemaEvidenceCount(record.symbol_graph_scope_resolution_ready) +
      Objc3SemaEvidenceCount(record.method_lookup_override_conflict_ready) +
      Objc3SemaEvidenceCount(record.property_synthesis_ivar_binding_ready) +
      Objc3SemaEvidenceCount(
          record.id_class_sel_object_pointer_type_checking_ready);
  record.failed_validation_count =
      record.required_validation_count >= record.passed_validation_count
          ? (record.required_validation_count -
             record.passed_validation_count)
          : record.required_validation_count;
  record.deterministic =
      Objc3SemaOwnerIsExplicit(
          record.control_binding_parity_validation_readiness_owner) &&
      Objc3SemaOwnerIsExplicit(record.stage_input_owner) &&
      Objc3SemaOwnerIsExplicit(record.typed_semantic_handoff_owner) &&
      record.owner_model == kObjc3SemaNoRetiredRouteOwnerModel &&
      record.strict_no_retired_route && record.strict_no_compatibility &&
      record.required_validation_count == 6u &&
      record.passed_validation_count == record.required_validation_count &&
      record.failed_validation_count == 0u;
  return record;
}
