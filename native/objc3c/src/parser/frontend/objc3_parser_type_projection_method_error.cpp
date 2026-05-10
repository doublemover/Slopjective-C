#include "parser/frontend/objc3_parser_type_projection_internal.h"

namespace objc3c::parse {

void CopyObjc3MethodReturnTypeErrorProjection(const FunctionDecl &source,
                                              Objc3MethodDecl &target) {
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
}

}  // namespace objc3c::parse
