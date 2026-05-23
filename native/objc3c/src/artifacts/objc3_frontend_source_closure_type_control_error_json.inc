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
      << ",\"fail_closed_construct_ids\":"
      << BuildStringArrayJson(summary.fail_closed_construct_ids)
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
      << ",\"value_optional_type_signature_sites\":"
      << summary.value_optional_type_signature_sites
      << ",\"value_optional_type_signature_source_supported\":"
      << (summary.value_optional_type_signature_source_supported ? "true" : "false")
      << ",\"value_optional_semantic_type_admission_supported\":"
      << (summary.value_optional_semantic_type_admission_supported ? "true"
                                                                   : "false")
      << ",\"value_optional_stable_layout_contract_supported\":"
      << (summary.value_optional_stable_layout_contract_supported ? "true"
                                                                  : "false")
      << ",\"value_optional_binding_narrowing_contract_supported\":"
      << (summary.value_optional_binding_narrowing_contract_supported ? "true"
                                                                      : "false")
      << ",\"value_optional_interface_roundtrip_supported\":"
      << (summary.value_optional_interface_roundtrip_supported ? "true"
                                                              : "false")
      << ",\"value_optional_runtime_execution_fail_closed\":"
      << (summary.value_optional_runtime_execution_fail_closed ? "true" : "false")
      << ",\"value_optional_issue_ref\":"
      << summary.value_optional_issue_ref
      << ",\"value_optional_canonical_spelling\":\""
      << EscapeJsonString(summary.value_optional_canonical_spelling)
      << "\",\"value_optional_reserved_diagnostic_code\":\""
      << EscapeJsonString(summary.value_optional_reserved_diagnostic_code)
      << "\",\"lowercase_optional_alias_diagnostic_code\":\""
      << EscapeJsonString(summary.lowercase_optional_alias_diagnostic_code)
      << "\",\"lowercase_optional_alias_rejected\":"
      << (summary.lowercase_optional_alias_rejected ? "true" : "false")
      << ",\"value_optional_nil_to_scalar_coercion_allowed\":"
      << (summary.value_optional_nil_to_scalar_coercion_allowed ? "true"
                                                                : "false")
      << ",\"value_optional_implicit_nil_absence_allowed\":"
      << (summary.value_optional_implicit_nil_absence_allowed ? "true"
                                                             : "false")
      << ",\"value_optional_nullable_pointer_conversion_allowed\":"
      << (summary.value_optional_nullable_pointer_conversion_allowed ? "true"
                                                                     : "false")
      << ",\"value_optional_throws_conversion_allowed\":"
      << (summary.value_optional_throws_conversion_allowed ? "true" : "false")
      << ",\"value_optional_abi_status\":\""
      << EscapeJsonString(summary.value_optional_abi_status)
      << "\",\"value_optional_interface_roundtrip_status\":\""
      << EscapeJsonString(summary.value_optional_interface_roundtrip_status)
      << "\""
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
      << ",\"guarded_match_pattern_sites\":"
      << summary.guarded_match_pattern_sites
      << ",\"match_result_case_pattern_sites\":"
      << summary.match_result_case_pattern_sites
      << ",\"match_expression_sites\":"
      << summary.match_expression_sites
      << ",\"match_expression_arm_sites\":"
      << summary.match_expression_arm_sites
      << ",\"match_expression_guard_sites\":"
      << summary.match_expression_guard_sites
      << ",\"guarded_match_issue_ref\":"
      << summary.guarded_match_issue_ref
      << ",\"guarded_match_admitted_syntax\":\""
      << EscapeJsonString(summary.guarded_match_admitted_syntax)
      << "\",\"guarded_match_condition_bool_required\":"
      << (summary.guarded_match_condition_bool_required ? "true" : "false")
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
      << ",\"guarded_match_pattern_source_supported\":"
      << (summary.guarded_match_pattern_source_supported ? "true" : "false")
      << ",\"match_result_case_pattern_source_supported\":"
      << (summary.match_result_case_pattern_source_supported ? "true"
                                                             : "false")
      << ",\"match_expression_source_supported\":"
      << (summary.match_expression_source_supported ? "true" : "false")
      << ",\"defer_keyword_reserved\":"
      << (summary.defer_keyword_reserved ? "true" : "false")
      << ",\"defer_fail_closed\":"
      << (summary.defer_fail_closed ? "true" : "false")
      << ",\"match_expression_fail_closed\":"
      << (summary.match_expression_fail_closed ? "true" : "false")
      << ",\"match_expression_result_typing_supported\":"
      << (summary.match_expression_result_typing_supported ? "true" : "false")
      << ",\"match_fat_arrow_arms_supported\":"
      << (summary.match_fat_arrow_arms_supported ? "true" : "false")
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
      << ",\"typed_throws_declaration_sites\":"
      << summary.typed_throws_declaration_sites
      << ",\"typed_throws_payload_arity_supported\":"
      << summary.typed_throws_payload_arity_supported
      << ",\"throws_declaration_source_supported\":"
      << (summary.throws_declaration_source_supported ? "true" : "false")
      << ",\"typed_throws_source_supported\":"
      << (summary.typed_throws_source_supported ? "true" : "false")
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
      << ",\"typed_throws_fail_closed\":"
      << (summary.typed_throws_fail_closed ? "true" : "false")
      << ",\"typed_throws_abi_lowering_fail_closed\":"
      << (summary.typed_throws_abi_lowering_fail_closed ? "true" : "false")
      << ",\"typed_throws_issue_ref\":"
      << summary.typed_throws_issue_ref
      << ",\"typed_throws_canonical_syntax\":\""
      << EscapeJsonString(summary.typed_throws_canonical_syntax)
      << "\",\"typed_throws_reserved_diagnostic_code\":\""
      << EscapeJsonString(summary.typed_throws_reserved_diagnostic_code)
      << "\",\"typed_throws_single_payload_reserved\":"
      << (summary.typed_throws_single_payload_reserved ? "true" : "false")
      << ",\"typed_throws_empty_payload_rejected\":"
      << (summary.typed_throws_empty_payload_rejected ? "true" : "false")
      << ",\"typed_throws_multi_payload_rejected\":"
      << (summary.typed_throws_multi_payload_rejected ? "true" : "false")
      << ",\"typed_throws_silent_erasure_allowed\":"
      << (summary.typed_throws_silent_erasure_allowed ? "true" : "false")
      << ",\"typed_throws_effect_record_status\":\""
      << EscapeJsonString(summary.typed_throws_effect_record_status)
      << "\",\"typed_throws_abi_status\":\""
      << EscapeJsonString(summary.typed_throws_abi_status)
      << "\",\"typed_throws_interface_roundtrip_status\":\""
      << EscapeJsonString(summary.typed_throws_interface_roundtrip_status)
      << "\""
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
