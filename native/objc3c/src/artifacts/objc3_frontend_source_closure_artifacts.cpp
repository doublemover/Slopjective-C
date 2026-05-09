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

std::string BuildConcurrencyAsyncSourceClosureSummaryJson(
    const Objc3FrontendConcurrencyAsyncSourceClosureSummary &summary) {
  std::ostringstream out;
  out << "{"
      << "\"contract_id\":\"" << summary.contract_id
      << "\",\"frontend_surface_path\":\"" << summary.frontend_surface_path
      << "\",\"source_model\":\"" << summary.source_model
      << "\",\"failure_model\":\"" << summary.failure_model
      << "\",\"source_only_claim_ids\":"
      << BuildStringArrayJson(summary.source_only_claim_ids)
      << ",\"async_keyword_sites\":" << summary.async_keyword_sites
      << ",\"async_function_sites\":" << summary.async_function_sites
      << ",\"async_method_sites\":" << summary.async_method_sites
      << ",\"await_keyword_sites\":" << summary.await_keyword_sites
      << ",\"await_expression_sites\":" << summary.await_expression_sites
      << ",\"executor_attribute_sites\":"
      << summary.executor_attribute_sites
      << ",\"executor_main_sites\":" << summary.executor_main_sites
      << ",\"executor_global_sites\":" << summary.executor_global_sites
      << ",\"executor_named_sites\":" << summary.executor_named_sites
      << ",\"async_function_source_supported\":"
      << (summary.async_function_source_supported ? "true" : "false")
      << ",\"async_method_source_supported\":"
      << (summary.async_method_source_supported ? "true" : "false")
      << ",\"await_expression_source_supported\":"
      << (summary.await_expression_source_supported ? "true" : "false")
      << ",\"executor_attribute_source_supported\":"
      << (summary.executor_attribute_source_supported ? "true" : "false")
      << ",\"deterministic_handoff\":"
      << (summary.deterministic_handoff ? "true" : "false")
      << ",\"ready_for_semantic_expansion\":"
      << (summary.ready_for_semantic_expansion ? "true" : "false")
      << ",\"replay_key\":\"" << EscapeJsonString(summary.replay_key)
      << "\"}";
  return out.str();
}

std::string BuildOwnershipSystemExtensionSourceClosureSummaryJson(
    const Objc3FrontendOwnershipSystemExtensionSourceClosureSummary &summary) {
  std::ostringstream out;
  out << "{"
      << "\"contract_id\":\"" << summary.contract_id
      << "\",\"frontend_surface_path\":\"" << summary.frontend_surface_path
      << "\",\"source_model\":\"" << summary.source_model
      << "\",\"failure_model\":\"" << summary.failure_model
      << "\",\"source_only_claim_ids\":"
      << BuildStringArrayJson(summary.source_only_claim_ids)
      << ",\"resource_attribute_sites\":" << summary.resource_attribute_sites
      << ",\"resource_close_clause_sites\":"
      << summary.resource_close_clause_sites
      << ",\"resource_invalid_clause_sites\":"
      << summary.resource_invalid_clause_sites
      << ",\"borrowed_pointer_sites\":" << summary.borrowed_pointer_sites
      << ",\"returns_borrowed_attribute_sites\":"
      << summary.returns_borrowed_attribute_sites
      << ",\"explicit_capture_list_sites\":"
      << summary.explicit_capture_list_sites
      << ",\"explicit_capture_item_sites\":"
      << summary.explicit_capture_item_sites
      << ",\"explicit_capture_weak_sites\":"
      << summary.explicit_capture_weak_sites
      << ",\"explicit_capture_unowned_sites\":"
      << summary.explicit_capture_unowned_sites
      << ",\"explicit_capture_move_sites\":"
      << summary.explicit_capture_move_sites
      << ",\"explicit_capture_plain_sites\":"
      << summary.explicit_capture_plain_sites
      << ",\"resource_attribute_source_supported\":"
      << (summary.resource_attribute_source_supported ? "true" : "false")
      << ",\"borrowed_pointer_source_supported\":"
      << (summary.borrowed_pointer_source_supported ? "true" : "false")
      << ",\"returns_borrowed_source_supported\":"
      << (summary.returns_borrowed_source_supported ? "true" : "false")
      << ",\"explicit_capture_list_source_supported\":"
      << (summary.explicit_capture_list_source_supported ? "true" : "false")
      << ",\"deterministic_handoff\":"
      << (summary.deterministic_handoff ? "true" : "false")
      << ",\"ready_for_semantic_expansion\":"
      << (summary.ready_for_semantic_expansion ? "true" : "false")
      << ",\"replay_key\":\"" << EscapeJsonString(summary.replay_key)
      << "\"}";
  return out.str();
}

std::string BuildOwnershipCleanupResourceCaptureSourceCompletionSummaryJson(
    const Objc3FrontendOwnershipCleanupResourceCaptureSourceCompletionSummary
        &summary) {
  std::ostringstream out;
  out << "{"
      << "\"contract_id\":\"" << summary.contract_id
      << "\",\"frontend_surface_path\":\"" << summary.frontend_surface_path
      << "\",\"source_model\":\"" << summary.source_model
      << "\",\"failure_model\":\"" << summary.failure_model
      << "\",\"source_only_claim_ids\":"
      << BuildStringArrayJson(summary.source_only_claim_ids)
      << ",\"cleanup_attribute_sites\":" << summary.cleanup_attribute_sites
      << ",\"cleanup_sugar_sites\":" << summary.cleanup_sugar_sites
      << ",\"resource_attribute_sites\":" << summary.resource_attribute_sites
      << ",\"resource_sugar_sites\":" << summary.resource_sugar_sites
      << ",\"resource_close_clause_sites\":"
      << summary.resource_close_clause_sites
      << ",\"resource_invalid_clause_sites\":"
      << summary.resource_invalid_clause_sites
      << ",\"explicit_capture_list_sites\":"
      << summary.explicit_capture_list_sites
      << ",\"explicit_capture_item_sites\":"
      << summary.explicit_capture_item_sites
      << ",\"explicit_capture_weak_sites\":"
      << summary.explicit_capture_weak_sites
      << ",\"explicit_capture_unowned_sites\":"
      << summary.explicit_capture_unowned_sites
      << ",\"explicit_capture_move_sites\":"
      << summary.explicit_capture_move_sites
      << ",\"explicit_capture_plain_sites\":"
      << summary.explicit_capture_plain_sites
      << ",\"cleanup_attribute_source_supported\":"
      << (summary.cleanup_attribute_source_supported ? "true" : "false")
      << ",\"resource_sugar_source_supported\":"
      << (summary.resource_sugar_source_supported ? "true" : "false")
      << ",\"explicit_capture_list_source_supported\":"
      << (summary.explicit_capture_list_source_supported ? "true" : "false")
      << ",\"deterministic_handoff\":"
      << (summary.deterministic_handoff ? "true" : "false")
      << ",\"ready_for_semantic_expansion\":"
      << (summary.ready_for_semantic_expansion ? "true" : "false")
      << ",\"replay_key\":\"" << EscapeJsonString(summary.replay_key)
      << "\"}";
  return out.str();
}

std::string BuildOwnershipRetainableCFamilySourceCompletionSummaryJson(
    const Objc3FrontendOwnershipRetainableCFamilySourceCompletionSummary
        &summary) {
  std::ostringstream out;
  out << "{"
      << "\"contract_id\":\"" << summary.contract_id
      << "\",\"frontend_surface_path\":\"" << summary.frontend_surface_path
      << "\",\"source_model\":\"" << summary.source_model
      << "\",\"failure_model\":\"" << summary.failure_model
      << "\",\"source_only_claim_ids\":"
      << BuildStringArrayJson(summary.source_only_claim_ids)
      << ",\"family_retain_sites\":" << summary.family_retain_sites
      << ",\"family_release_sites\":" << summary.family_release_sites
      << ",\"family_autorelease_sites\":"
      << summary.family_autorelease_sites
      << ",\"compatibility_returns_retained_sites\":"
      << summary.compatibility_returns_retained_sites
      << ",\"compatibility_returns_not_retained_sites\":"
      << summary.compatibility_returns_not_retained_sites
      << ",\"compatibility_consumed_sites\":"
      << summary.compatibility_consumed_sites
      << ",\"callable_annotation_source_supported\":"
      << (summary.callable_annotation_source_supported ? "true" : "false")
      << ",\"compatibility_alias_source_supported\":"
      << (summary.compatibility_alias_source_supported ? "true" : "false")
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

}  // namespace objc3::artifacts::frontend
