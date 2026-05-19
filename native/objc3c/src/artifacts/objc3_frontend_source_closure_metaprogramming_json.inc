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
