std::string BuildMetaprogrammingMacroSafetySandboxDeterminismSummaryJson(
    const Objc3MetaprogrammingMacroSafetySandboxDeterminismSummary &summary) {
  std::ostringstream out;
  out << "{"
      << "\"contract_id\":\"" << EscapeJsonString(summary.contract_id)
      << "\",\"semantic_dependency_contract_id\":\""
      << EscapeJsonString(summary.semantic_dependency_contract_id)
      << "\",\"surface_path\":\"" << EscapeJsonString(summary.surface_path)
      << "\",\"semantic_model\":\"" << EscapeJsonString(summary.semantic_model)
      << "\",\"deferred_model\":\"" << EscapeJsonString(summary.deferred_model)
      << "\",\"macro_marker_sites\":" << summary.macro_marker_sites
      << ",\"macro_package_sites\":" << summary.macro_package_sites
      << ",\"macro_provenance_sites\":" << summary.macro_provenance_sites
      << ",\"expansion_visible_macro_sites\":"
      << summary.expansion_visible_macro_sites
      << ",\"safe_macro_callable_sites\":"
      << summary.safe_macro_callable_sites
      << ",\"incomplete_macro_metadata_sites\":"
      << summary.incomplete_macro_metadata_sites
      << ",\"orphan_macro_metadata_sites\":"
      << summary.orphan_macro_metadata_sites
      << ",\"invalid_package_sites\":" << summary.invalid_package_sites
      << ",\"invalid_provenance_sites\":"
      << summary.invalid_provenance_sites
      << ",\"nondeterministic_callable_sites\":"
      << summary.nondeterministic_callable_sites
      << ",\"unsupported_callable_topology_sites\":"
      << summary.unsupported_callable_topology_sites
      << ",\"semantic_dependency_required\":"
      << (summary.semantic_dependency_required ? "true" : "false")
      << ",\"metadata_completeness_enforced\":"
      << (summary.metadata_completeness_enforced ? "true" : "false")
      << ",\"sandbox_namespace_enforced\":"
      << (summary.sandbox_namespace_enforced ? "true" : "false")
      << ",\"provenance_determinism_enforced\":"
      << (summary.provenance_determinism_enforced ? "true" : "false")
      << ",\"callable_determinism_enforced\":"
      << (summary.callable_determinism_enforced ? "true" : "false")
      << ",\"macro_execution_deferred\":"
      << (summary.macro_execution_deferred ? "true" : "false")
      << ",\"deterministic\":" << (summary.deterministic ? "true" : "false")
      << ",\"ready_for_lowering_and_runtime\":"
      << (summary.ready_for_lowering_and_runtime ? "true" : "false")
      << ",\"failure_reason\":\"" << EscapeJsonString(summary.failure_reason)
      << "\",\"replay_key\":\"" << EscapeJsonString(summary.replay_key)
      << "\"}";
  return out.str();
}
