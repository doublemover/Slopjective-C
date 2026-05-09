#include "artifacts/objc3_frontend_control_flow_semantic_artifacts.h"

#include <sstream>
#include <string>

#include "io/objc3_json.h"

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

}  // namespace objc3::artifacts::frontend
