#include "pipeline/frontend_source_closure_replay_keys.h"

#include <sstream>

namespace objc3c::pipeline::orchestration {

std::string BuildTypeSystemTypeSourceClosureReplayKey(
    const Objc3FrontendTypeSystemTypeSourceClosureSummary &summary) {
  std::ostringstream out;
  out << summary.contract_id
      << ";protocol_required_methods=" << summary.protocol_required_method_count
      << ";protocol_optional_methods=" << summary.protocol_optional_method_count
      << ";protocol_required_properties="
      << summary.protocol_required_property_count
      << ";protocol_optional_properties="
      << summary.protocol_optional_property_count
      << ";object_pointer_type_spelling_sites="
      << summary.object_pointer_type_spelling_sites
      << ";pointer_declarator_entries=" << summary.pointer_declarator_entries
      << ";nullability_suffix_entries=" << summary.nullability_suffix_entries
      << ";generic_suffix_entries=" << summary.generic_suffix_entries
      << ";optional_sites=" << summary.optional_binding_sites << ":"
      << summary.guard_binding_sites << ":" << summary.optional_send_sites << ":"
      << summary.nil_coalescing_sites << ":"
      << summary.typed_keypath_literal_sites
      << ";optional_member_access_sites="
      << summary.optional_member_access_sites
      << ";value_optional_type_signatures="
      << summary.value_optional_type_signature_sites
      << ";value_optional_semantic_type_admission="
      << (summary.value_optional_semantic_type_admission_supported ? "true"
                                                                   : "false")
      << ";value_optional_stable_layout_contract="
      << (summary.value_optional_stable_layout_contract_supported ? "true"
                                                                  : "false")
      << ";value_optional_binding_narrowing_contract="
      << (summary.value_optional_binding_narrowing_contract_supported ? "true"
                                                                      : "false")
      << ";value_optional_interface_roundtrip_contract="
      << (summary.value_optional_interface_roundtrip_supported ? "true"
                                                               : "false")
      << ";value_optional_runtime_fail_closed="
      << (summary.value_optional_runtime_execution_fail_closed ? "true" : "false")
      << ";value_optional_implicit_nil_absence_allowed="
      << (summary.value_optional_implicit_nil_absence_allowed ? "true" : "false")
      << ";value_optional_reserved_diagnostic="
      << summary.value_optional_reserved_diagnostic_code
      << ";lowercase_optional_alias_diagnostic="
      << summary.lowercase_optional_alias_diagnostic_code
      << ";value_optional_interface_roundtrip="
      << summary.value_optional_interface_roundtrip_status
      << ";deterministic="
      << (summary.deterministic_handoff ? "true" : "false");
  return out.str();
}

std::string BuildControlFlowControlFlowSourceClosureReplayKey(
    const Objc3FrontendControlFlowControlFlowSourceClosureSummary &summary) {
  std::ostringstream out;
  out << summary.contract_id
      << ";guard_binding_sites=" << summary.guard_binding_sites
      << ";guard_binding_clause_sites=" << summary.guard_binding_clause_sites
      << ";guard_boolean_condition_sites="
      << summary.guard_boolean_condition_sites
      << ";switch_pattern_sites=" << summary.switch_case_pattern_sites << ":"
      << summary.switch_default_pattern_sites
      << ";match_surface_sites=" << summary.match_statement_sites << ":"
      << summary.match_case_pattern_sites << ":" << summary.match_default_sites
      << ":" << summary.match_wildcard_pattern_sites << ":"
      << summary.match_literal_pattern_sites << ":"
      << summary.match_binding_pattern_sites << ":"
      << summary.guarded_match_pattern_sites << ":"
      << summary.match_result_case_pattern_sites
      << ";match_expression_sites=" << summary.match_expression_sites << ":"
      << summary.match_expression_arm_sites << ":"
      << summary.match_expression_guard_sites
      << ";guarded_match_issue_ref=" << summary.guarded_match_issue_ref
      << ";guarded_match_admitted_syntax="
      << summary.guarded_match_admitted_syntax
      << ";guarded_match_condition_bool_required="
      << (summary.guarded_match_condition_bool_required ? "true" : "false")
      << ";match_expression_fail_closed="
      << (summary.match_expression_fail_closed ? "true" : "false")
      << ";match_expression_result_typing_supported="
      << (summary.match_expression_result_typing_supported ? "true" : "false")
      << ";match_fat_arrow_arms_supported="
      << (summary.match_fat_arrow_arms_supported ? "true" : "false")
      << ";reserved_keyword_sites=" << summary.defer_keyword_sites
      << ";deterministic="
      << (summary.deterministic_handoff ? "true" : "false");
  return out.str();
}

std::string BuildErrorHandlingErrorSourceClosureReplayKey(
    const Objc3FrontendErrorHandlingErrorSourceClosureSummary &summary) {
  std::ostringstream out;
  out << summary.contract_id
      << ";throws_sites=" << summary.function_throws_declaration_sites << ":"
      << summary.method_throws_declaration_sites
      << ";result_sites=" << summary.result_like_sites << ":"
      << summary.result_success_sites << ":" << summary.result_failure_sites
      << ":" << summary.result_branch_sites << ":"
      << summary.result_payload_sites
      << ";nserror_sites=" << summary.ns_error_bridging_sites << ":"
      << summary.ns_error_out_parameter_sites << ":"
      << summary.ns_error_bridge_path_sites
      << ";bridge_marker_sites=" << summary.objc_nserror_attribute_sites << ":"
      << summary.objc_status_code_attribute_sites << ":"
      << summary.status_code_success_clause_sites << ":"
      << summary.status_code_error_type_clause_sites << ":"
      << summary.status_code_mapping_clause_sites
      << ";reserved_keyword_sites=" << summary.try_keyword_sites << ":"
      << summary.throw_keyword_sites << ":" << summary.catch_keyword_sites
      << ";typed_throws_fail_closed="
      << (summary.typed_throws_fail_closed ? "true" : "false")
      << ";typed_throws_source_supported="
      << (summary.typed_throws_source_supported ? "true" : "false")
      << ";typed_throws_declaration_sites="
      << summary.typed_throws_declaration_sites
      << ";typed_throws_abi_lowering_fail_closed="
      << (summary.typed_throws_abi_lowering_fail_closed ? "true" : "false")
      << ";typed_throws_reserved_diagnostic="
      << summary.typed_throws_reserved_diagnostic_code
      << ";typed_throws_abi_status=" << summary.typed_throws_abi_status
      << ";typed_throws_interface_roundtrip="
      << summary.typed_throws_interface_roundtrip_status
      << ";typed_throws_catch_compatibility="
      << summary.typed_throws_catch_compatibility_status
      << ";typed_throws_bridge_to_id_error="
      << summary.typed_throws_bridge_to_id_error_policy
      << ";typed_throws_foreign_carrier_fail_closed="
      << (summary.typed_throws_foreign_carrier_fail_closed ? "true"
                                                           : "false")
      << ";deterministic="
      << (summary.deterministic_handoff ? "true" : "false");
  return out.str();
}

}  // namespace objc3c::pipeline::orchestration
