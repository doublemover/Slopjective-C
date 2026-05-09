#include "artifacts/objc3_frontend_error_semantic_artifacts.h"

#include <sstream>
#include <string>

#include "io/objc3_json.h"

namespace objc3::artifacts::frontend {
namespace {

using objc3::io::EscapeJsonString;

}  // namespace

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

}  // namespace objc3::artifacts::frontend
