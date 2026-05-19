std::string BuildMetaprogrammingExpansionBehaviorSemanticModelSummaryJson(
    const Objc3MetaprogrammingExpansionBehaviorSemanticModelSummary &summary) {
  std::ostringstream out;
  out << "{"
      << "\"contract_id\":\"" << EscapeJsonString(summary.contract_id)
      << "\",\"frontend_dependency_contract_id\":\""
      << EscapeJsonString(summary.frontend_dependency_contract_id)
      << "\",\"surface_path\":\"" << EscapeJsonString(summary.surface_path)
      << "\",\"semantic_model\":\"" << EscapeJsonString(summary.semantic_model)
      << "\",\"deferred_model\":\"" << EscapeJsonString(summary.deferred_model)
      << "\",\"derive_marker_sites\":" << summary.derive_marker_sites
      << ",\"macro_marker_sites\":" << summary.macro_marker_sites
      << ",\"macro_package_sites\":" << summary.macro_package_sites
      << ",\"macro_provenance_sites\":" << summary.macro_provenance_sites
      << ",\"expansion_visible_macro_sites\":"
      << summary.expansion_visible_macro_sites
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
      << ",\"property_behavior_contract_violation_sites\":"
      << summary.property_behavior_contract_violation_sites
      << ",\"source_dependency_required\":"
      << (summary.source_dependency_required ? "true" : "false")
      << ",\"derive_macro_source_supported\":"
      << (summary.derive_macro_source_supported ? "true" : "false")
      << ",\"macro_package_provenance_surface_reused\":"
      << (summary.macro_package_provenance_surface_reused ? "true" : "false")
      << ",\"property_behavior_source_supported\":"
      << (summary.property_behavior_source_supported ? "true" : "false")
      << ",\"synthesized_visibility_surface_reused\":"
      << (summary.synthesized_visibility_surface_reused ? "true" : "false")
      << ",\"derive_synthesis_deferred\":"
      << (summary.derive_synthesis_deferred ? "true" : "false")
      << ",\"macro_execution_deferred\":"
      << (summary.macro_execution_deferred ? "true" : "false")
      << ",\"property_behavior_runtime_deferred\":"
      << (summary.property_behavior_runtime_deferred ? "true" : "false")
      << ",\"deterministic\":" << (summary.deterministic ? "true" : "false")
      << ",\"ready_for_core_implementation\":"
      << (summary.ready_for_core_implementation ? "true" : "false")
      << ",\"failure_reason\":\"" << EscapeJsonString(summary.failure_reason)
      << "\",\"replay_key\":\"" << EscapeJsonString(summary.replay_key)
      << "\"}";
  return out.str();
}
