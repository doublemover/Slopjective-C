#include "artifacts/objc3_frontend_control_flow_semantic_artifacts.h"

#include <sstream>
#include <string>

#include "io/objc3_json.h"
#include "lower/contracts/control_flow_safety_lowering_contracts.h"
#include "sema/objc3_sema_contract_effects_flow_control_flow.h"

namespace objc3::artifacts::frontend {
namespace {

using objc3::io::EscapeJsonString;

}  // namespace

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
      << ",\"match_expression_semantic_sites\":"
      << summary.match_expression_semantic_sites
      << ",\"match_expression_result_type_sites\":"
      << summary.match_expression_result_type_sites
      << ",\"match_expression_guard_condition_sites\":"
      << summary.match_expression_guard_condition_sites
      << ",\"match_expression_binding_scope_sites\":"
      << summary.match_expression_binding_scope_sites
      << ",\"match_expression_result_case_scope_sites\":"
      << summary.match_expression_result_case_scope_sites
      << ",\"match_expression_exhaustive_sites\":"
      << summary.match_expression_exhaustive_sites
      << ",\"match_expression_non_exhaustive_diagnostic_sites\":"
      << summary.match_expression_non_exhaustive_diagnostic_sites
      << ",\"match_expression_lowering_eligible_sites\":"
      << summary.match_expression_lowering_eligible_sites
      << ",\"match_expression_guard_effect_fail_closed_sites\":"
      << summary.match_expression_guard_effect_fail_closed_sites
      << ",\"match_expression_result_type_mismatch_sites\":"
      << summary.match_expression_result_type_mismatch_sites
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

std::string BuildControlFlowControlFlowSafetyLoweringContractJson(
    const Objc3ControlFlowControlFlowSafetyLoweringContract &contract,
    const Objc3ControlFlowControlFlowSemanticModelSummary &semantic_summary,
    const std::string &semantic_summary_replay_key,
    const std::string &replay_key) {
  std::ostringstream out;
  const bool source_semantic_model_ready =
      semantic_summary.deterministic &&
      semantic_summary.ready_for_lowering_and_runtime;
  out << "{"
      << "\"contract_id\":\""
      << EscapeJsonString(kObjc3ControlFlowControlFlowSafetyLoweringContractId)
      << "\",\"surface_path\":\""
      << EscapeJsonString(kObjc3ControlFlowControlFlowSafetyLoweringSurfacePath)
      << "\",\"source_semantic_contract_id\":\""
      << EscapeJsonString(semantic_summary.contract_id)
      << "\",\"guard_model\":\""
      << EscapeJsonString(kObjc3ControlFlowControlFlowSafetyLoweringGuardModel)
      << "\",\"match_model\":\""
      << EscapeJsonString(kObjc3ControlFlowControlFlowSafetyLoweringMatchModel)
      << "\",\"defer_model\":\""
      << EscapeJsonString(kObjc3ControlFlowControlFlowSafetyLoweringDeferModel)
      << "\",\"authority_model\":\""
      << EscapeJsonString(kObjc3ControlFlowControlFlowSafetyLoweringAuthorityModel)
      << "\",\"fail_closed_model\":\""
      << EscapeJsonString(kObjc3ControlFlowControlFlowSafetyLoweringFailClosedModel)
      << "\",\"guard_statement_sites\":"
      << contract.guard_statement_sites
      << ",\"guard_clause_sites\":" << contract.guard_clause_sites
      << ",\"match_statement_sites\":" << contract.match_statement_sites
      << ",\"match_expression_sites\":" << contract.match_expression_sites
      << ",\"defer_statement_sites\":" << contract.defer_statement_sites
      << ",\"live_guard_short_circuit_sites\":"
      << contract.live_guard_short_circuit_sites
      << ",\"live_match_dispatch_sites\":"
      << contract.live_match_dispatch_sites
      << ",\"live_match_expression_dispatch_sites\":"
      << contract.live_match_expression_dispatch_sites
      << ",\"live_defer_cleanup_sites\":"
      << contract.live_defer_cleanup_sites
      << ",\"fail_closed_guard_short_circuit_sites\":"
      << contract.fail_closed_guard_short_circuit_sites
      << ",\"fail_closed_match_dispatch_sites\":"
      << contract.fail_closed_match_dispatch_sites
      << ",\"fail_closed_match_expression_dispatch_sites\":"
      << contract.fail_closed_match_expression_dispatch_sites
      << ",\"fail_closed_defer_cleanup_sites\":"
      << contract.fail_closed_defer_cleanup_sites
      << ",\"deterministic_fail_closed_sites\":"
      << contract.deterministic_fail_closed_sites
      << ",\"contract_violation_sites\":"
      << contract.contract_violation_sites
      << ",\"deterministic\":"
      << (contract.deterministic ? "true" : "false")
      << ",\"fail_closed\":"
      << (contract.deterministic_fail_closed_sites > 0u ? "true" : "false")
      << ",\"source_semantic_model_ready\":"
      << (source_semantic_model_ready ? "true" : "false")
      << ",\"ready_for_native_guard_lowering\":true"
      << ",\"ready_for_native_match_lowering\":"
      << (contract.fail_closed_match_dispatch_sites == 0u &&
                  contract.fail_closed_match_expression_dispatch_sites == 0u
              ? "true"
              : "false")
      << ",\"ready_for_native_defer_lowering\":true"
      << ",\"semantic_summary_replay_key\":\""
      << EscapeJsonString(semantic_summary_replay_key)
      << "\",\"replay_key\":\"" << EscapeJsonString(replay_key)
      << "\"}";
  return out.str();
}

}  // namespace objc3::artifacts::frontend
