std::string BuildToolingDiagnosticsMigratorSourceInventorySummaryJson(
    const Objc3FrontendToolingDiagnosticsMigratorSourceInventorySummary
        &summary) {
  std::ostringstream out;
  out << "{"
      << "\"contract_id\":\"" << EscapeJsonString(summary.contract_id)
      << "\",\"frontend_surface_path\":\""
      << EscapeJsonString(summary.frontend_surface_path)
      << "\",\"source_model\":\"" << EscapeJsonString(summary.source_model)
      << "\",\"failure_model\":\"" << EscapeJsonString(summary.failure_model)
      << "\",\"dependency_contract_ids\":"
      << BuildStringArrayJson(summary.dependency_contract_ids)
      << ",\"source_only_claim_ids\":"
      << BuildStringArrayJson(summary.source_only_claim_ids)
      << ",\"advanced_feature_family_count\":"
      << summary.advanced_feature_family_count
      << ",\"dependency_surface_count\":"
      << summary.dependency_surface_count
      << ",\"aggregated_source_only_claim_count\":"
      << summary.aggregated_source_only_claim_count
      << ",\"fail_closed_construct_count\":"
      << summary.fail_closed_construct_count
      << ",\"diagnostic_surface_sites\":"
      << summary.diagnostic_surface_sites
      << ",\"fixit_surface_sites\":" << summary.fixit_surface_sites
      << ",\"migrator_surface_sites\":" << summary.migrator_surface_sites
      << ",\"canonicalization_hint_sites\":"
      << summary.canonicalization_hint_sites
      << ",\"error_surface_sites\":" << summary.error_surface_sites
      << ",\"concurrency_surface_sites\":"
      << summary.concurrency_surface_sites
      << ",\"system_surface_sites\":" << summary.system_surface_sites
      << ",\"dispatch_surface_sites\":" << summary.dispatch_surface_sites
      << ",\"metaprogramming_surface_sites\":"
      << summary.metaprogramming_surface_sites
      << ",\"interop_surface_sites\":" << summary.interop_surface_sites
      << ",\"diagnostics_inventory_source_supported\":"
      << (summary.diagnostics_inventory_source_supported ? "true" : "false")
      << ",\"fixit_inventory_source_supported\":"
      << (summary.fixit_inventory_source_supported ? "true" : "false")
      << ",\"migrator_inventory_source_supported\":"
      << (summary.migrator_inventory_source_supported ? "true" : "false")
      << ",\"deterministic_handoff\":"
      << (summary.deterministic_handoff ? "true" : "false")
      << ",\"ready_for_semantic_expansion\":"
      << (summary.ready_for_semantic_expansion ? "true" : "false")
      << ",\"replay_key\":\"" << EscapeJsonString(summary.replay_key)
      << "\"}";
  return out.str();
}
