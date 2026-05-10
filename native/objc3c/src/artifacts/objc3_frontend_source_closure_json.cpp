#include "artifacts/objc3_frontend_source_closure_artifacts.h"

#include <sstream>
#include <string>
#include <vector>

#include "io/json/json_writer.h"
#include "io/objc3_json.h"

namespace objc3::artifacts::frontend {
namespace {

using objc3::io::EscapeJsonString;

std::string BuildStringArrayJson(const std::vector<std::string> &values) {
  return objc3::io::json::RenderJsonStringArray(values);
}

}  // namespace

std::string BuildTypeSystemTypeSourceClosureSummaryJson(
    const Objc3FrontendTypeSystemTypeSourceClosureSummary &summary) {
  std::ostringstream out;
  out << "{"
      << "\"contract_id\":\"" << summary.contract_id
      << "\",\"frontend_surface_path\":\"" << summary.frontend_surface_path
      << "\",\"source_model\":\"" << summary.source_model
      << "\",\"failure_model\":\"" << summary.failure_model
      << "\",\"source_only_claim_ids\":"
      << BuildStringArrayJson(summary.source_only_claim_ids)
      << ",\"unsupported_claim_ids\":"
      << BuildStringArrayJson(summary.unsupported_claim_ids)
      << ",\"protocol_required_method_count\":"
      << summary.protocol_required_method_count
      << ",\"protocol_optional_method_count\":"
      << summary.protocol_optional_method_count
      << ",\"protocol_required_property_count\":"
      << summary.protocol_required_property_count
      << ",\"protocol_optional_property_count\":"
      << summary.protocol_optional_property_count
      << ",\"object_pointer_type_spelling_sites\":"
      << summary.object_pointer_type_spelling_sites
      << ",\"pointer_declarator_entries\":"
      << summary.pointer_declarator_entries
      << ",\"nullability_suffix_entries\":"
      << summary.nullability_suffix_entries
      << ",\"generic_suffix_entries\":" << summary.generic_suffix_entries
      << ",\"optional_binding_sites\":" << summary.optional_binding_sites
      << ",\"guard_binding_sites\":" << summary.guard_binding_sites
      << ",\"optional_send_sites\":" << summary.optional_send_sites
      << ",\"optional_member_access_sites\":"
      << summary.optional_member_access_sites
      << ",\"nil_coalescing_sites\":" << summary.nil_coalescing_sites
      << ",\"typed_keypath_literal_sites\":"
      << summary.typed_keypath_literal_sites
      << ",\"protocol_optional_partition_source_supported\":"
      << (summary.protocol_optional_partition_source_supported ? "true"
                                                               : "false")
      << ",\"object_pointer_nullability_source_supported\":"
      << (summary.object_pointer_nullability_source_supported ? "true"
                                                              : "false")
      << ",\"pragmatic_generic_suffix_source_supported\":"
      << (summary.pragmatic_generic_suffix_source_supported ? "true" : "false")
      << ",\"optional_binding_source_supported\":"
      << (summary.optional_binding_source_supported ? "true" : "false")
      << ",\"optional_send_source_supported\":"
      << (summary.optional_send_source_supported ? "true" : "false")
      << ",\"nil_coalescing_source_supported\":"
      << (summary.nil_coalescing_source_supported ? "true" : "false")
      << ",\"typed_keypath_literal_source_supported\":"
      << (summary.typed_keypath_literal_source_supported ? "true" : "false")
      << ",\"optional_member_access_fail_closed\":"
      << (summary.optional_member_access_fail_closed ? "true" : "false")
      << ",\"nil_coalescing_fail_closed\":"
      << (summary.nil_coalescing_fail_closed ? "true" : "false")
      << ",\"typed_keypath_literal_fail_closed\":"
      << (summary.typed_keypath_literal_fail_closed ? "true" : "false")
      << ",\"deterministic_handoff\":"
      << (summary.deterministic_handoff ? "true" : "false")
      << ",\"ready_for_semantic_expansion\":"
      << (summary.ready_for_semantic_expansion ? "true" : "false")
      << ",\"replay_key\":\"" << EscapeJsonString(summary.replay_key)
      << "\"}";
  return out.str();
}

std::string BuildControlFlowControlFlowSourceClosureSummaryJson(
    const Objc3FrontendControlFlowControlFlowSourceClosureSummary &summary) {
  std::ostringstream out;
  out << "{"
      << "\"contract_id\":\"" << summary.contract_id
      << "\",\"frontend_surface_path\":\"" << summary.frontend_surface_path
      << "\",\"source_model\":\"" << summary.source_model
      << "\",\"failure_model\":\"" << summary.failure_model
      << "\",\"supported_construct_ids\":"
      << BuildStringArrayJson(summary.supported_construct_ids)
      << ",\"fail_closed_construct_ids\":"
      << BuildStringArrayJson(summary.fail_closed_construct_ids)
      << ",\"guard_binding_sites\":" << summary.guard_binding_sites
      << ",\"guard_binding_clause_sites\":"
      << summary.guard_binding_clause_sites
      << ",\"guard_boolean_condition_sites\":"
      << summary.guard_boolean_condition_sites
      << ",\"switch_case_pattern_sites\":"
      << summary.switch_case_pattern_sites
      << ",\"switch_default_pattern_sites\":"
      << summary.switch_default_pattern_sites
      << ",\"defer_keyword_sites\":" << summary.defer_keyword_sites
      << ",\"match_statement_sites\":" << summary.match_statement_sites
      << ",\"match_case_pattern_sites\":"
      << summary.match_case_pattern_sites
      << ",\"match_default_sites\":" << summary.match_default_sites
      << ",\"match_wildcard_pattern_sites\":"
      << summary.match_wildcard_pattern_sites
      << ",\"match_literal_pattern_sites\":"
      << summary.match_literal_pattern_sites
      << ",\"match_binding_pattern_sites\":"
      << summary.match_binding_pattern_sites
      << ",\"match_result_case_pattern_sites\":"
      << summary.match_result_case_pattern_sites
      << ",\"guard_binding_source_supported\":"
      << (summary.guard_binding_source_supported ? "true" : "false")
      << ",\"guard_condition_list_source_supported\":"
      << (summary.guard_condition_list_source_supported ? "true" : "false")
      << ",\"switch_case_pattern_source_supported\":"
      << (summary.switch_case_pattern_source_supported ? "true" : "false")
      << ",\"defer_statement_source_supported\":"
      << (summary.defer_statement_source_supported ? "true" : "false")
      << ",\"match_statement_source_supported\":"
      << (summary.match_statement_source_supported ? "true" : "false")
      << ",\"match_wildcard_pattern_source_supported\":"
      << (summary.match_wildcard_pattern_source_supported ? "true" : "false")
      << ",\"match_literal_pattern_source_supported\":"
      << (summary.match_literal_pattern_source_supported ? "true" : "false")
      << ",\"match_binding_pattern_source_supported\":"
      << (summary.match_binding_pattern_source_supported ? "true" : "false")
      << ",\"match_result_case_pattern_source_supported\":"
      << (summary.match_result_case_pattern_source_supported ? "true"
                                                             : "false")
      << ",\"defer_keyword_reserved\":"
      << (summary.defer_keyword_reserved ? "true" : "false")
      << ",\"defer_fail_closed\":"
      << (summary.defer_fail_closed ? "true" : "false")
      << ",\"match_expression_fail_closed\":"
      << (summary.match_expression_fail_closed ? "true" : "false")
      << ",\"guarded_pattern_fail_closed\":"
      << (summary.guarded_pattern_fail_closed ? "true" : "false")
      << ",\"type_test_pattern_fail_closed\":"
      << (summary.type_test_pattern_fail_closed ? "true" : "false")
      << ",\"deterministic_handoff\":"
      << (summary.deterministic_handoff ? "true" : "false")
      << ",\"ready_for_semantic_expansion\":"
      << (summary.ready_for_semantic_expansion ? "true" : "false")
      << ",\"replay_key\":\"" << EscapeJsonString(summary.replay_key)
      << "\"}";
  return out.str();
}

std::string BuildErrorHandlingErrorSourceClosureSummaryJson(
    const Objc3FrontendErrorHandlingErrorSourceClosureSummary &summary) {
  std::ostringstream out;
  out << "{"
      << "\"contract_id\":\"" << summary.contract_id
      << "\",\"frontend_surface_path\":\"" << summary.frontend_surface_path
      << "\",\"source_model\":\"" << summary.source_model
      << "\",\"failure_model\":\"" << summary.failure_model
      << "\",\"source_only_claim_ids\":"
      << BuildStringArrayJson(summary.source_only_claim_ids)
      << ",\"fail_closed_construct_ids\":"
      << BuildStringArrayJson(summary.fail_closed_construct_ids)
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
      << ",\"try_keyword_sites\":" << summary.try_keyword_sites
      << ",\"throw_keyword_sites\":" << summary.throw_keyword_sites
      << ",\"catch_keyword_sites\":" << summary.catch_keyword_sites
      << ",\"throws_declaration_source_supported\":"
      << (summary.throws_declaration_source_supported ? "true" : "false")
      << ",\"result_carrier_source_supported\":"
      << (summary.result_carrier_source_supported ? "true" : "false")
      << ",\"ns_error_bridging_source_supported\":"
      << (summary.ns_error_bridging_source_supported ? "true" : "false")
      << ",\"error_bridge_marker_source_supported\":"
      << (summary.error_bridge_marker_source_supported ? "true" : "false")
      << ",\"try_keyword_reserved\":"
      << (summary.try_keyword_reserved ? "true" : "false")
      << ",\"throw_keyword_reserved\":"
      << (summary.throw_keyword_reserved ? "true" : "false")
      << ",\"catch_keyword_reserved\":"
      << (summary.catch_keyword_reserved ? "true" : "false")
      << ",\"try_fail_closed\":"
      << (summary.try_fail_closed ? "true" : "false")
      << ",\"throw_fail_closed\":"
      << (summary.throw_fail_closed ? "true" : "false")
      << ",\"do_catch_fail_closed\":"
      << (summary.do_catch_fail_closed ? "true" : "false")
      << ",\"deterministic_handoff\":"
      << (summary.deterministic_handoff ? "true" : "false")
      << ",\"ready_for_semantic_expansion\":"
      << (summary.ready_for_semantic_expansion ? "true" : "false")
      << ",\"replay_key\":\"" << EscapeJsonString(summary.replay_key)
      << "\"}";
  return out.str();
}

std::string BuildDispatchDispatchIntentSourceClosureSummaryJson(
    const Objc3FrontendDispatchDispatchIntentSourceClosureSummary &summary) {
  std::ostringstream out;
  out << "{"
      << "\"contract_id\":\"" << summary.contract_id
      << "\",\"frontend_surface_path\":\"" << summary.frontend_surface_path
      << "\",\"source_model\":\"" << summary.source_model
      << "\",\"failure_model\":\"" << summary.failure_model
      << "\",\"source_only_claim_ids\":"
      << BuildStringArrayJson(summary.source_only_claim_ids)
      << ",\"direct_callable_sites\":" << summary.direct_callable_sites
      << ",\"final_callable_sites\":" << summary.final_callable_sites
      << ",\"dynamic_callable_sites\":" << summary.dynamic_callable_sites
      << ",\"direct_members_container_sites\":"
      << summary.direct_members_container_sites
      << ",\"final_container_sites\":" << summary.final_container_sites
      << ",\"sealed_container_sites\":" << summary.sealed_container_sites
      << ",\"actor_container_sites\":" << summary.actor_container_sites
      << ",\"callable_annotation_source_supported\":"
      << (summary.callable_annotation_source_supported ? "true" : "false")
      << ",\"container_annotation_source_supported\":"
      << (summary.container_annotation_source_supported ? "true" : "false")
      << ",\"deterministic_handoff\":"
      << (summary.deterministic_handoff ? "true" : "false")
      << ",\"ready_for_semantic_expansion\":"
      << (summary.ready_for_semantic_expansion ? "true" : "false")
      << ",\"replay_key\":\"" << EscapeJsonString(summary.replay_key)
      << "\"}";
  return out.str();
}

std::string BuildDispatchDispatchIntentSourceCompletionSummaryJson(
    const Objc3FrontendDispatchDispatchIntentSourceCompletionSummary &summary) {
  std::ostringstream out;
  out << "{"
      << "\"contract_id\":\"" << summary.contract_id
      << "\",\"frontend_surface_path\":\"" << summary.frontend_surface_path
      << "\",\"source_model\":\"" << summary.source_model
      << "\",\"failure_model\":\"" << summary.failure_model
      << "\",\"source_only_claim_ids\":"
      << BuildStringArrayJson(summary.source_only_claim_ids)
      << ",\"prefixed_container_attribute_sites\":"
      << summary.prefixed_container_attribute_sites
      << ",\"direct_members_container_sites\":"
      << summary.direct_members_container_sites
      << ",\"final_container_sites\":" << summary.final_container_sites
      << ",\"sealed_container_sites\":" << summary.sealed_container_sites
      << ",\"effective_direct_member_sites\":"
      << summary.effective_direct_member_sites
      << ",\"direct_members_defaulted_method_sites\":"
      << summary.direct_members_defaulted_method_sites
      << ",\"direct_members_dynamic_opt_out_sites\":"
      << summary.direct_members_dynamic_opt_out_sites
      << ",\"prefixed_attribute_source_supported\":"
      << (summary.prefixed_attribute_source_supported ? "true" : "false")
      << ",\"defaulting_source_supported\":"
      << (summary.defaulting_source_supported ? "true" : "false")
      << ",\"deterministic_handoff\":"
      << (summary.deterministic_handoff ? "true" : "false")
      << ",\"ready_for_semantic_expansion\":"
      << (summary.ready_for_semantic_expansion ? "true" : "false")
      << ",\"replay_key\":\"" << EscapeJsonString(summary.replay_key)
      << "\"}";
  return out.str();
}

std::string BuildMetaprogrammingMetaprogrammingSourceClosureSummaryJson(
    const Objc3FrontendMetaprogrammingMetaprogrammingSourceClosureSummary
        &summary) {
  std::ostringstream out;
  out << "{"
      << "\"contract_id\":\"" << summary.contract_id
      << "\",\"frontend_surface_path\":\"" << summary.frontend_surface_path
      << "\",\"source_model\":\"" << summary.source_model
      << "\",\"failure_model\":\"" << summary.failure_model
      << "\",\"source_only_claim_ids\":"
      << BuildStringArrayJson(summary.source_only_claim_ids)
      << ",\"derive_marker_sites\":" << summary.derive_marker_sites
      << ",\"macro_marker_sites\":" << summary.macro_marker_sites
      << ",\"property_behavior_sites\":" << summary.property_behavior_sites
      << ",\"derive_marker_source_supported\":"
      << (summary.derive_marker_source_supported ? "true" : "false")
      << ",\"macro_marker_source_supported\":"
      << (summary.macro_marker_source_supported ? "true" : "false")
      << ",\"property_behavior_source_supported\":"
      << (summary.property_behavior_source_supported ? "true" : "false")
      << ",\"deterministic_handoff\":"
      << (summary.deterministic_handoff ? "true" : "false")
      << ",\"ready_for_semantic_expansion\":"
      << (summary.ready_for_semantic_expansion ? "true" : "false")
      << ",\"replay_key\":\"" << EscapeJsonString(summary.replay_key)
      << "\"}";
  return out.str();
}

std::string
BuildMetaprogrammingMacroPackageProvenanceSourceCompletionSummaryJson(
    const Objc3FrontendMetaprogrammingMacroPackageProvenanceSourceCompletionSummary
        &summary) {
  std::ostringstream out;
  out << "{"
      << "\"contract_id\":\"" << summary.contract_id
      << "\",\"frontend_surface_path\":\"" << summary.frontend_surface_path
      << "\",\"source_model\":\"" << summary.source_model
      << "\",\"failure_model\":\"" << summary.failure_model
      << "\",\"source_only_claim_ids\":"
      << BuildStringArrayJson(summary.source_only_claim_ids)
      << ",\"macro_marker_sites\":" << summary.macro_marker_sites
      << ",\"macro_package_sites\":" << summary.macro_package_sites
      << ",\"macro_provenance_sites\":" << summary.macro_provenance_sites
      << ",\"macro_cache_key_sites\":" << summary.macro_cache_key_sites
      << ",\"macro_sandbox_policy_sites\":"
      << summary.macro_sandbox_policy_sites
      << ",\"expansion_visible_macro_sites\":"
      << summary.expansion_visible_macro_sites
      << ",\"macro_package_source_supported\":"
      << (summary.macro_package_source_supported ? "true" : "false")
      << ",\"macro_provenance_source_supported\":"
      << (summary.macro_provenance_source_supported ? "true" : "false")
      << ",\"macro_cache_key_source_supported\":"
      << (summary.macro_cache_key_source_supported ? "true" : "false")
      << ",\"macro_sandbox_policy_source_supported\":"
      << (summary.macro_sandbox_policy_source_supported ? "true" : "false")
      << ",\"expansion_visible_source_supported\":"
      << (summary.expansion_visible_source_supported ? "true" : "false")
      << ",\"deterministic_handoff\":"
      << (summary.deterministic_handoff ? "true" : "false")
      << ",\"ready_for_semantic_expansion\":"
      << (summary.ready_for_semantic_expansion ? "true" : "false")
      << ",\"replay_key\":\"" << EscapeJsonString(summary.replay_key)
      << "\"}";
  return out.str();
}

std::string BuildMetaprogrammingPropertyBehaviorSourceCompletionSummaryJson(
    const Objc3FrontendMetaprogrammingPropertyBehaviorSourceCompletionSummary
        &summary) {
  std::ostringstream out;
  out << "{"
      << "\"contract_id\":\"" << EscapeJsonString(summary.contract_id)
      << "\",\"frontend_surface_path\":\""
      << EscapeJsonString(summary.frontend_surface_path)
      << "\",\"source_model\":\"" << EscapeJsonString(summary.source_model)
      << "\",\"failure_model\":\"" << EscapeJsonString(summary.failure_model)
      << "\",\"source_only_claim_ids\":"
      << BuildStringArrayJson(summary.source_only_claim_ids)
      << ",\"property_behavior_sites\":" << summary.property_behavior_sites
      << ",\"interface_property_behavior_sites\":"
      << summary.interface_property_behavior_sites
      << ",\"implementation_property_behavior_sites\":"
      << summary.implementation_property_behavior_sites
      << ",\"protocol_property_behavior_sites\":"
      << summary.protocol_property_behavior_sites
      << ",\"synthesized_binding_visible_sites\":"
      << summary.synthesized_binding_visible_sites
      << ",\"synthesized_getter_visible_sites\":"
      << summary.synthesized_getter_visible_sites
      << ",\"synthesized_setter_visible_sites\":"
      << summary.synthesized_setter_visible_sites
      << ",\"property_behavior_source_supported\":"
      << (summary.property_behavior_source_supported ? "true" : "false")
      << ",\"synthesized_declaration_visibility_supported\":"
      << (summary.synthesized_declaration_visibility_supported ? "true"
                                                               : "false")
      << ",\"deterministic_handoff\":"
      << (summary.deterministic_handoff ? "true" : "false")
      << ",\"ready_for_semantic_expansion\":"
      << (summary.ready_for_semantic_expansion ? "true" : "false")
      << ",\"replay_key\":\"" << EscapeJsonString(summary.replay_key)
      << "\"}";
  return out.str();
}

std::string BuildInteropForeignImportSourceClosureSummaryJson(
    const Objc3FrontendInteropForeignImportSourceClosureSummary &summary) {
  std::ostringstream out;
  out << "{"
      << "\"contract_id\":\"" << EscapeJsonString(summary.contract_id)
      << "\",\"frontend_surface_path\":\""
      << EscapeJsonString(summary.frontend_surface_path)
      << "\",\"source_model\":\"" << EscapeJsonString(summary.source_model)
      << "\",\"failure_model\":\"" << EscapeJsonString(summary.failure_model)
      << "\",\"source_only_claim_ids\":"
      << BuildStringArrayJson(summary.source_only_claim_ids)
      << ",\"foreign_callable_sites\":" << summary.foreign_callable_sites
      << ",\"extern_foreign_callable_sites\":"
      << summary.extern_foreign_callable_sites
      << ",\"import_module_annotation_sites\":"
      << summary.import_module_annotation_sites
      << ",\"imported_module_name_sites\":"
      << summary.imported_module_name_sites
      << ",\"export_header_annotation_sites\":"
      << summary.export_header_annotation_sites
      << ",\"export_header_name_sites\":"
      << summary.export_header_name_sites
      << ",\"mixed_image_annotation_sites\":"
      << summary.mixed_image_annotation_sites
      << ",\"mixed_image_name_sites\":" << summary.mixed_image_name_sites
      << ",\"package_entry_annotation_sites\":"
      << summary.package_entry_annotation_sites
      << ",\"package_entry_name_sites\":"
      << summary.package_entry_name_sites
      << ",\"interop_annotation_sites\":" << summary.interop_annotation_sites
      << ",\"foreign_declaration_source_supported\":"
      << (summary.foreign_declaration_source_supported ? "true" : "false")
      << ",\"imported_surface_source_supported\":"
      << (summary.imported_surface_source_supported ? "true" : "false")
      << ",\"interop_annotation_source_supported\":"
      << (summary.interop_annotation_source_supported ? "true" : "false")
      << ",\"deterministic_handoff\":"
      << (summary.deterministic_handoff ? "true" : "false")
      << ",\"ready_for_semantic_expansion\":"
      << (summary.ready_for_semantic_expansion ? "true" : "false")
      << ",\"replay_key\":\"" << EscapeJsonString(summary.replay_key)
      << "\"}";
  return out.str();
}

std::string BuildInteropCppSwiftInteropAnnotationSourceCompletionSummaryJson(
    const Objc3FrontendInteropCppSwiftInteropAnnotationSourceCompletionSummary
        &summary) {
  std::ostringstream out;
  out << "{"
      << "\"contract_id\":\"" << EscapeJsonString(summary.contract_id)
      << "\",\"frontend_surface_path\":\""
      << EscapeJsonString(summary.frontend_surface_path)
      << "\",\"source_model\":\"" << EscapeJsonString(summary.source_model)
      << "\",\"failure_model\":\"" << EscapeJsonString(summary.failure_model)
      << "\",\"source_only_claim_ids\":"
      << BuildStringArrayJson(summary.source_only_claim_ids)
      << ",\"swift_name_annotation_sites\":"
      << summary.swift_name_annotation_sites
      << ",\"swift_private_annotation_sites\":"
      << summary.swift_private_annotation_sites
      << ",\"cpp_name_annotation_sites\":" << summary.cpp_name_annotation_sites
      << ",\"header_name_annotation_sites\":"
      << summary.header_name_annotation_sites
      << ",\"abi_alignment_annotation_sites\":"
      << summary.abi_alignment_annotation_sites
      << ",\"foreign_type_annotation_sites\":"
      << summary.foreign_type_annotation_sites
      << ",\"interop_metadata_annotation_sites\":"
      << summary.interop_metadata_annotation_sites
      << ",\"named_annotation_payload_sites\":"
      << summary.named_annotation_payload_sites
      << ",\"swift_annotation_source_supported\":"
      << (summary.swift_annotation_source_supported ? "true" : "false")
      << ",\"cpp_annotation_source_supported\":"
      << (summary.cpp_annotation_source_supported ? "true" : "false")
      << ",\"interop_metadata_source_supported\":"
      << (summary.interop_metadata_source_supported ? "true" : "false")
      << ",\"deterministic_handoff\":"
      << (summary.deterministic_handoff ? "true" : "false")
      << ",\"ready_for_semantic_expansion\":"
      << (summary.ready_for_semantic_expansion ? "true" : "false")
      << ",\"replay_key\":\"" << EscapeJsonString(summary.replay_key)
      << "\"}";
  return out.str();
}

}  // namespace objc3::artifacts::frontend
