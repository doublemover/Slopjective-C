#include "artifacts/objc3_frontend_error_semantic_artifacts.h"

#include <algorithm>
#include <cstddef>
#include <sstream>
#include <string>

#include "io/objc3_json.h"

namespace objc3::artifacts::frontend {
namespace {

using objc3::io::EscapeJsonString;

}  // namespace

Objc3ThrowsPropagationLoweringContract
BuildThrowsPropagationLoweringContract(
    const Objc3SemaParityContractSurface &sema_parity_surface) {
  Objc3ThrowsPropagationLoweringContract contract;
  const std::size_t raw_sites =
      sema_parity_surface.throws_propagation_sites_total;
  const std::size_t raw_namespace_segment_sites =
      sema_parity_surface.throws_propagation_namespace_segment_sites_total;
  const std::size_t raw_import_edge_sites =
      sema_parity_surface.throws_propagation_import_edge_candidate_sites_total;
  const std::size_t raw_object_pointer_sites =
      sema_parity_surface.throws_propagation_object_pointer_type_sites_total;
  const std::size_t raw_pointer_declarator_sites =
      sema_parity_surface.throws_propagation_pointer_declarator_sites_total;
  const std::size_t raw_normalized_sites =
      sema_parity_surface.throws_propagation_normalized_sites_total;
  const std::size_t raw_cache_candidate_sites =
      sema_parity_surface
          .throws_propagation_cache_invalidation_candidate_sites_total;
  const std::size_t raw_violation_sites =
      sema_parity_surface.throws_propagation_contract_violation_sites_total;

  contract.throws_propagation_sites =
      std::max({raw_sites, raw_namespace_segment_sites, raw_import_edge_sites,
                raw_pointer_declarator_sites, raw_normalized_sites,
                raw_cache_candidate_sites, raw_violation_sites});
  contract.namespace_segment_sites =
      std::min(raw_namespace_segment_sites, contract.throws_propagation_sites);
  contract.import_edge_candidate_sites =
      std::min(raw_import_edge_sites, contract.throws_propagation_sites);
  contract.object_pointer_type_sites =
      std::max(raw_object_pointer_sites, contract.import_edge_candidate_sites);
  contract.pointer_declarator_sites =
      std::min(raw_pointer_declarator_sites, contract.throws_propagation_sites);
  contract.normalized_sites =
      std::min(raw_normalized_sites, contract.throws_propagation_sites);
  const std::size_t normalized_budget =
      (contract.throws_propagation_sites >= contract.normalized_sites)
          ? (contract.throws_propagation_sites - contract.normalized_sites)
          : 0;
  contract.cache_invalidation_candidate_sites =
      std::min(raw_cache_candidate_sites, normalized_budget);
  contract.contract_violation_sites =
      std::min(raw_violation_sites, contract.throws_propagation_sites);
  contract.deterministic =
      sema_parity_surface.throws_propagation_summary.deterministic &&
      sema_parity_surface.deterministic_throws_propagation_handoff &&
      contract.contract_violation_sites == 0 &&
      contract.normalized_sites == contract.throws_propagation_sites;
  return contract;
}

Objc3ResultLikeLoweringContract BuildResultLikeLoweringContract(
    const Objc3SemaParityContractSurface &sema_parity_surface) {
  Objc3ResultLikeLoweringContract contract;
  const std::size_t raw_sites =
      sema_parity_surface.result_like_lowering_sites_total;
  const std::size_t raw_success_sites =
      sema_parity_surface.result_like_lowering_result_success_sites_total;
  const std::size_t raw_failure_sites =
      sema_parity_surface.result_like_lowering_result_failure_sites_total;
  const std::size_t raw_branch_sites =
      sema_parity_surface.result_like_lowering_result_branch_sites_total;
  const std::size_t raw_payload_sites =
      sema_parity_surface.result_like_lowering_result_payload_sites_total;
  const std::size_t raw_normalized_sites =
      sema_parity_surface.result_like_lowering_normalized_sites_total;
  const std::size_t raw_branch_merge_sites =
      sema_parity_surface.result_like_lowering_branch_merge_sites_total;
  const std::size_t raw_violation_sites =
      sema_parity_surface.result_like_lowering_contract_violation_sites_total;

  contract.result_like_sites =
      std::max({raw_sites, raw_success_sites, raw_failure_sites,
                raw_branch_sites, raw_payload_sites, raw_normalized_sites,
                raw_branch_merge_sites, raw_violation_sites});
  contract.result_success_sites =
      std::min(raw_success_sites, contract.result_like_sites);
  contract.result_failure_sites =
      std::min(raw_failure_sites, contract.result_like_sites);
  contract.result_branch_sites =
      std::min(raw_branch_sites, contract.result_like_sites);
  contract.result_payload_sites =
      std::min(raw_payload_sites, contract.result_like_sites);
  contract.normalized_sites =
      std::min(raw_normalized_sites, contract.result_like_sites);
  const std::size_t normalized_budget =
      (contract.result_like_sites >= contract.normalized_sites)
          ? (contract.result_like_sites - contract.normalized_sites)
          : 0;
  contract.branch_merge_sites =
      std::min(raw_branch_merge_sites, normalized_budget);
  contract.contract_violation_sites =
      std::min(raw_violation_sites, contract.result_like_sites);
  contract.deterministic =
      sema_parity_surface.deterministic_result_like_lowering_handoff &&
      contract.contract_violation_sites == 0 &&
      contract.normalized_sites + contract.branch_merge_sites ==
          contract.result_like_sites;
  return contract;
}

Objc3NSErrorBridgingLoweringContract BuildNSErrorBridgingLoweringContract(
    const Objc3SemaParityContractSurface &sema_parity_surface) {
  Objc3NSErrorBridgingLoweringContract contract;
  const std::size_t raw_sites =
      sema_parity_surface.ns_error_bridging_sites_total;
  const std::size_t raw_parameter_sites =
      sema_parity_surface.ns_error_bridging_ns_error_parameter_sites_total;
  const std::size_t raw_out_parameter_sites =
      sema_parity_surface.ns_error_bridging_ns_error_out_parameter_sites_total;
  const std::size_t raw_bridge_path_sites =
      sema_parity_surface.ns_error_bridging_ns_error_bridge_path_sites_total;
  const std::size_t raw_failable_call_sites =
      sema_parity_surface.ns_error_bridging_failable_call_sites_total;
  const std::size_t raw_normalized_sites =
      sema_parity_surface.ns_error_bridging_normalized_sites_total;
  const std::size_t raw_bridge_boundary_sites =
      sema_parity_surface.ns_error_bridging_bridge_boundary_sites_total;
  const std::size_t raw_violation_sites =
      sema_parity_surface.ns_error_bridging_contract_violation_sites_total;

  contract.ns_error_bridging_sites =
      std::max({raw_sites, raw_parameter_sites, raw_out_parameter_sites,
                raw_bridge_path_sites, raw_failable_call_sites,
                raw_normalized_sites, raw_bridge_boundary_sites,
                raw_violation_sites});
  contract.ns_error_parameter_sites =
      std::min(raw_parameter_sites, contract.ns_error_bridging_sites);
  contract.ns_error_out_parameter_sites =
      std::min(raw_out_parameter_sites, contract.ns_error_parameter_sites);
  contract.ns_error_bridge_path_sites =
      std::min(raw_bridge_path_sites, contract.ns_error_out_parameter_sites);
  contract.failable_call_sites =
      std::min(raw_failable_call_sites, contract.ns_error_bridging_sites);
  contract.normalized_sites =
      std::min(raw_normalized_sites, contract.ns_error_bridging_sites);
  const std::size_t normalized_budget =
      (contract.ns_error_bridging_sites >= contract.normalized_sites)
          ? (contract.ns_error_bridging_sites - contract.normalized_sites)
          : 0;
  contract.bridge_boundary_sites =
      std::min(raw_bridge_boundary_sites, normalized_budget);
  contract.contract_violation_sites =
      std::min(raw_violation_sites, contract.ns_error_bridging_sites);
  contract.deterministic =
      sema_parity_surface.deterministic_ns_error_bridging_handoff &&
      contract.contract_violation_sites == 0 &&
      contract.normalized_sites + contract.bridge_boundary_sites ==
          contract.ns_error_bridging_sites;
  return contract;
}

Objc3UnwindCleanupLoweringContract BuildUnwindCleanupLoweringContract(
    const Objc3SemaParityContractSurface &sema_parity_surface) {
  Objc3UnwindCleanupLoweringContract contract;
  const std::size_t raw_sites =
      sema_parity_surface.unwind_cleanup_sites_total;
  const std::size_t raw_exceptional_exit_sites =
      sema_parity_surface.unwind_cleanup_exceptional_exit_sites_total;
  const std::size_t raw_action_sites =
      sema_parity_surface.unwind_cleanup_action_sites_total;
  const std::size_t raw_scope_sites =
      sema_parity_surface.unwind_cleanup_scope_sites_total;
  const std::size_t raw_resume_sites =
      sema_parity_surface.unwind_cleanup_resume_sites_total;
  const std::size_t raw_normalized_sites =
      sema_parity_surface.unwind_cleanup_normalized_sites_total;
  const std::size_t raw_fail_closed_sites =
      sema_parity_surface.unwind_cleanup_fail_closed_sites_total;
  const std::size_t raw_violation_sites =
      sema_parity_surface.unwind_cleanup_contract_violation_sites_total;

  contract.unwind_cleanup_sites =
      std::max({raw_sites, raw_exceptional_exit_sites, raw_action_sites,
                raw_scope_sites, raw_resume_sites, raw_normalized_sites,
                raw_fail_closed_sites, raw_violation_sites});
  contract.unwind_edge_sites =
      std::min(raw_exceptional_exit_sites, contract.unwind_cleanup_sites);
  contract.cleanup_scope_sites =
      std::min(raw_scope_sites, contract.unwind_cleanup_sites);
  contract.cleanup_emit_sites =
      std::min(raw_action_sites, contract.cleanup_scope_sites);
  contract.landing_pad_sites = 0;
  contract.cleanup_resume_sites =
      std::min(raw_resume_sites, contract.unwind_cleanup_sites);
  contract.normalized_sites =
      std::min(raw_normalized_sites, contract.unwind_cleanup_sites);
  const std::size_t normalized_budget =
      (contract.unwind_cleanup_sites >= contract.normalized_sites)
          ? (contract.unwind_cleanup_sites - contract.normalized_sites)
          : 0;
  contract.guard_blocked_sites =
      std::min(raw_fail_closed_sites, normalized_budget);
  contract.contract_violation_sites =
      std::min(raw_violation_sites, contract.unwind_cleanup_sites);
  contract.deterministic =
      sema_parity_surface.deterministic_unwind_cleanup_handoff &&
      contract.contract_violation_sites == 0 &&
      contract.normalized_sites + contract.guard_blocked_sites ==
          contract.unwind_cleanup_sites;
  return contract;
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

std::string BuildErrorHandlingTryDoCatchSemanticSummaryJson(
    const Objc3ErrorHandlingTryDoCatchSemanticSummary &summary) {
  std::ostringstream out;
  out << "{"
      << "\"contract_id\":\"" << EscapeJsonString(summary.contract_id)
      << "\",\"dependency_contract_id\":\""
      << EscapeJsonString(summary.dependency_contract_id)
      << "\",\"surface_path\":\"" << EscapeJsonString(summary.surface_path)
      << "\",\"semantic_model\":\""
      << EscapeJsonString(summary.semantic_model)
      << "\",\"deferred_model\":\""
      << EscapeJsonString(summary.deferred_model)
      << "\",\"try_expression_sites\":" << summary.try_expression_sites
      << ",\"try_propagating_sites\":" << summary.try_propagating_sites
      << ",\"try_optional_sites\":" << summary.try_optional_sites
      << ",\"try_forced_sites\":" << summary.try_forced_sites
      << ",\"throw_statement_sites\":" << summary.throw_statement_sites
      << ",\"do_catch_sites\":" << summary.do_catch_sites
      << ",\"catch_clause_sites\":" << summary.catch_clause_sites
      << ",\"catch_binding_sites\":" << summary.catch_binding_sites
      << ",\"catch_all_sites\":" << summary.catch_all_sites
      << ",\"throwing_callable_try_sites\":"
      << summary.throwing_callable_try_sites
      << ",\"bridged_callable_try_sites\":" << summary.bridged_callable_try_sites
      << ",\"caller_propagation_sites\":" << summary.caller_propagation_sites
      << ",\"local_handler_sites\":" << summary.local_handler_sites
      << ",\"rethrow_sites\":" << summary.rethrow_sites
      << ",\"contract_violation_sites\":" << summary.contract_violation_sites
      << ",\"source_dependency_required\":"
      << (summary.source_dependency_required ? "true" : "false")
      << ",\"try_surface_landed\":"
      << (summary.try_surface_landed ? "true" : "false")
      << ",\"throw_surface_landed\":"
      << (summary.throw_surface_landed ? "true" : "false")
      << ",\"do_catch_surface_landed\":"
      << (summary.do_catch_surface_landed ? "true" : "false")
      << ",\"throwing_context_legality_enforced\":"
      << (summary.throwing_context_legality_enforced ? "true" : "false")
      << ",\"native_emit_remains_fail_closed\":"
      << (summary.native_emit_remains_fail_closed ? "true" : "false")
      << ",\"deterministic\":" << (summary.deterministic ? "true" : "false")
      << ",\"ready_for_lowering_and_runtime\":"
      << (summary.ready_for_lowering_and_runtime ? "true" : "false")
      << ",\"failure_reason\":\"" << EscapeJsonString(summary.failure_reason)
      << "\",\"replay_key\":\"" << EscapeJsonString(summary.replay_key)
      << "\"}";
  return out.str();
}

std::string BuildErrorHandlingErrorBridgeLegalitySummaryJson(
    const Objc3ErrorHandlingErrorBridgeLegalitySummary &summary) {
  std::ostringstream out;
  out << "{"
      << "\"contract_id\":\"" << EscapeJsonString(summary.contract_id)
      << "\",\"dependency_contract_id\":\""
      << EscapeJsonString(summary.dependency_contract_id)
      << "\",\"surface_path\":\"" << EscapeJsonString(summary.surface_path)
      << "\",\"semantic_model\":\""
      << EscapeJsonString(summary.semantic_model)
      << "\",\"deferred_model\":\""
      << EscapeJsonString(summary.deferred_model)
      << "\",\"bridge_callable_sites\":" << summary.bridge_callable_sites
      << ",\"objc_nserror_callable_sites\":"
      << summary.objc_nserror_callable_sites
      << ",\"objc_status_code_callable_sites\":"
      << summary.objc_status_code_callable_sites
      << ",\"semantically_valid_bridge_callable_sites\":"
      << summary.semantically_valid_bridge_callable_sites
      << ",\"try_eligible_bridge_callable_sites\":"
      << summary.try_eligible_bridge_callable_sites
      << ",\"missing_error_out_parameter_sites\":"
      << summary.missing_error_out_parameter_sites
      << ",\"invalid_nserror_return_sites\":"
      << summary.invalid_nserror_return_sites
      << ",\"invalid_status_return_sites\":"
      << summary.invalid_status_return_sites
      << ",\"invalid_error_type_sites\":"
      << summary.invalid_error_type_sites
      << ",\"missing_mapping_symbol_sites\":"
      << summary.missing_mapping_symbol_sites
      << ",\"invalid_mapping_signature_sites\":"
      << summary.invalid_mapping_signature_sites
      << ",\"throws_bridge_conflict_sites\":"
      << summary.throws_bridge_conflict_sites
      << ",\"marker_conflict_sites\":" << summary.marker_conflict_sites
      << ",\"unsupported_combination_sites\":"
      << summary.unsupported_combination_sites
      << ",\"contract_violation_sites\":"
      << summary.contract_violation_sites
      << ",\"source_dependency_required\":"
      << (summary.source_dependency_required ? "true" : "false")
      << ",\"bridge_legality_landed\":"
      << (summary.bridge_legality_landed ? "true" : "false")
      << ",\"try_bridge_filter_landed\":"
      << (summary.try_bridge_filter_landed ? "true" : "false")
      << ",\"unsupported_combinations_fail_closed\":"
      << (summary.unsupported_combinations_fail_closed ? "true" : "false")
      << ",\"native_emit_remains_fail_closed\":"
      << (summary.native_emit_remains_fail_closed ? "true" : "false")
      << ",\"deterministic\":" << (summary.deterministic ? "true" : "false")
      << ",\"ready_for_lowering_and_runtime\":"
      << (summary.ready_for_lowering_and_runtime ? "true" : "false")
      << ",\"failure_reason\":\"" << EscapeJsonString(summary.failure_reason)
      << "\",\"replay_key\":\"" << EscapeJsonString(summary.replay_key)
      << "\"}";
  return out.str();
}

}  // namespace objc3::artifacts::frontend
