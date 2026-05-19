std::string BuildToolingMigrationCanonicalizationSourceCompletionSummaryJson(
    const Objc3FrontendToolingMigrationCanonicalizationSourceCompletionSummary
        &summary) {
  std::ostringstream out;
  out << "{"
      << "\"contract_id\":\"" << EscapeJsonString(summary.contract_id)
      << "\",\"dependency_contract_id\":\""
      << EscapeJsonString(summary.dependency_contract_id)
      << "\",\"frontend_surface_path\":\""
      << EscapeJsonString(summary.frontend_surface_path)
      << "\",\"source_model\":\"" << EscapeJsonString(summary.source_model)
      << "\",\"completion_model\":\""
      << EscapeJsonString(summary.completion_model)
      << "\",\"failure_model\":\""
      << EscapeJsonString(summary.failure_model)
      << "\",\"source_only_claim_ids\":"
      << BuildStringArrayJson(summary.source_only_claim_ids)
      << ",\"language_profile\":\""
      << EscapeJsonString(summary.language_profile)
      << "\",\"canonical_literal_rejection_diagnostics_enabled\":"
      << (summary.canonical_literal_rejection_diagnostics_enabled ? "true"
                                                                  : "false")
      << ",\"canonical_literal_rejection_diagnostics_required\":"
      << (summary.canonical_literal_rejection_diagnostics_required ? "true"
                                                                  : "false")
      << ",\"canonical_yes_literal_rejection_sites\":"
      << summary.legacy_yes_sites
      << ",\"canonical_no_literal_rejection_sites\":"
      << summary.legacy_no_sites
      << ",\"canonical_null_literal_rejection_sites\":"
      << summary.legacy_null_sites
      << ",\"canonical_literal_rejection_total_sites\":"
      << summary.legacy_total_sites
      << ",\"canonical_true_rewrite_sites\":"
      << summary.canonical_true_rewrite_sites
      << ",\"canonical_false_rewrite_sites\":"
      << summary.canonical_false_rewrite_sites
      << ",\"canonical_nil_rewrite_sites\":"
      << summary.canonical_nil_rewrite_sites
      << ",\"canonicalization_candidate_sites\":"
      << summary.canonicalization_candidate_sites
      << ",\"fixit_candidate_sites\":" << summary.fixit_candidate_sites
      << ",\"migrator_candidate_sites\":"
      << summary.migrator_candidate_sites
      << ",\"dependency_inventory_ready\":"
      << (summary.dependency_inventory_ready ? "true" : "false")
      << ",\"canonicalization_surface_supported\":"
      << (summary.canonicalization_surface_supported ? "true" : "false")
      << ",\"fixit_migration_surface_supported\":"
      << (summary.fixit_migration_surface_supported ? "true" : "false")
      << ",\"deterministic_handoff\":"
      << (summary.deterministic_handoff ? "true" : "false")
      << ",\"ready_for_semantic_expansion\":"
      << (summary.ready_for_semantic_expansion ? "true" : "false")
      << ",\"failure_reason\":\"" << EscapeJsonString(summary.failure_reason)
      << "\",\"replay_key\":\"" << EscapeJsonString(summary.replay_key)
      << "\"}";
  return out.str();
}
