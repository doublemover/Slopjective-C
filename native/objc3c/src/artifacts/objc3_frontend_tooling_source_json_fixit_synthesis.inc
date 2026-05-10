std::string BuildToolingFeatureSpecificFixitSynthesisSummaryJson(
    const Objc3ToolingFeatureSpecificFixitSynthesisSummary &summary) {
  std::ostringstream out;
  out << "{"
      << "\"contract_id\":\"" << EscapeJsonString(summary.contract_id)
      << "\",\"dependency_contract_id\":\""
      << EscapeJsonString(summary.dependency_contract_id)
      << "\",\"frontend_surface_path\":\""
      << EscapeJsonString(summary.frontend_surface_path)
      << "\",\"semantic_model\":\""
      << EscapeJsonString(summary.semantic_model)
      << "\",\"fixit_family_ids\":"
      << BuildStringArrayJson(summary.fixit_family_ids)
      << ",\"fixit_family_count\":" << summary.fixit_family_count
      << ",\"migration_fixit_candidate_sites\":"
      << summary.migration_fixit_candidate_sites
      << ",\"migrator_candidate_sites\":" << summary.migrator_candidate_sites
      << ",\"ownership_arc_fixit_available_sites\":"
      << summary.ownership_arc_fixit_available_sites
      << ",\"ownership_arc_empty_fixit_hint_sites\":"
      << summary.ownership_arc_empty_fixit_hint_sites
      << ",\"diagnostic_taxonomy_ready\":"
      << (summary.diagnostic_taxonomy_ready ? "true" : "false")
      << ",\"deterministic_handoff\":"
      << (summary.deterministic_handoff ? "true" : "false")
      << ",\"ready_for_lowering_and_runtime\":"
      << (summary.ready_for_lowering_and_runtime ? "true" : "false")
      << ",\"failure_reason\":\"" << EscapeJsonString(summary.failure_reason)
      << "\",\"replay_key\":\"" << EscapeJsonString(summary.replay_key)
      << "\"}";
  return out.str();
}
