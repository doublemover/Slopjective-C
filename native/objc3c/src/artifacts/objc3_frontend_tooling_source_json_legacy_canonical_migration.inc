std::string BuildToolingLegacyCanonicalMigrationSemanticsReplayKey(
    const Objc3ToolingLegacyCanonicalMigrationSemanticsSummary &summary) {
  std::ostringstream out;
  out << "mode=" << summary.effective_language_profile
      << ";canonical-rejection-diagnostics="
      << (summary.canonical_literal_rejection_diagnostics_enabled ? "true"
                                                                  : "false")
      << ";canonical-literal-rejection-sites="
      << summary.current_run_legacy_literal_sites
      << ";candidates=" << summary.current_run_canonicalization_candidate_sites
      << ";families=" << summary.fixit_family_count
      << ";fail-closed=" << (summary.fail_closed ? "true" : "false")
      << ";rejection-code=" << summary.canonical_mode_rejection_code
      << ";ready="
      << (summary.ready_for_lowering_and_runtime ? "true" : "false");
  return out.str();
}

Objc3ToolingLegacyCanonicalMigrationSemanticsSummary
BuildToolingLegacyCanonicalMigrationSemanticsSummary(
    const Objc3FrontendCompatibilityStrictnessClaimSemanticsSummary
        &compatibility_summary,
    const Objc3ToolingFeatureSpecificFixitSynthesisSummary &fixit_summary) {
  Objc3ToolingLegacyCanonicalMigrationSemanticsSummary summary;
  summary.effective_language_profile =
      compatibility_summary.effective_language_profile;
  summary.canonical_literal_rejection_diagnostics_enabled =
      compatibility_summary.canonical_literal_rejection_diagnostics_enabled;
  summary.current_run_legacy_literal_sites =
      fixit_summary.migration_fixit_candidate_sites;
  summary.current_run_canonicalization_candidate_sites =
      fixit_summary.migration_fixit_candidate_sites;
  summary.fixit_family_count = fixit_summary.fixit_family_count;
  summary.fail_closed = compatibility_summary.fail_closed;
  summary.selected_configuration_valid =
      compatibility_summary.selected_configuration_valid;
  summary.language_profile_semantics_landed =
      compatibility_summary.language_profile_semantics_landed;
  summary.canonical_literal_rejection_semantics_landed =
      compatibility_summary.canonical_literal_rejection_semantics_landed;
  summary.canonical_mode_rejection_ready =
      compatibility_summary.fail_closed &&
      compatibility_summary.language_profile_semantics_landed &&
      compatibility_summary.canonical_literal_rejection_semantics_landed &&
      summary.canonical_mode_rejection_code ==
          kObjc3ToolingLegacyCanonicalMigrationDiagnosticCode;
  summary.feature_specific_fixit_surface_ready =
      fixit_summary.ready_for_lowering_and_runtime;
  summary.deterministic_handoff =
      IsReadyObjc3FrontendCompatibilityStrictnessClaimSemanticsSummary(
          compatibility_summary) &&
      summary.feature_specific_fixit_surface_ready &&
      summary.selected_configuration_valid &&
      !compatibility_summary.selected_configuration_downgraded &&
      !compatibility_summary.selected_configuration_rejected &&
      summary.canonical_mode_rejection_ready;
  summary.ready_for_lowering_and_runtime = summary.deterministic_handoff;
  if (!summary.feature_specific_fixit_surface_ready) {
    summary.failure_reason =
        "tooling feature-specific fix-it synthesis prerequisite is not ready";
  } else if (!IsReadyObjc3FrontendCompatibilityStrictnessClaimSemanticsSummary(
                 compatibility_summary)) {
    summary.failure_reason =
        "frontend canonical selection claim semantics prerequisite is not ready";
  }
  summary.replay_key =
      BuildToolingLegacyCanonicalMigrationSemanticsReplayKey(summary);
  return summary;
}

std::string BuildToolingLegacyCanonicalMigrationSemanticsSummaryJson(
    const Objc3ToolingLegacyCanonicalMigrationSemanticsSummary &summary) {
  std::ostringstream out;
  out << "{"
      << "\"contract_id\":\"" << EscapeJsonString(summary.contract_id)
      << "\",\"dependency_contract_id\":\""
      << EscapeJsonString(summary.dependency_contract_id)
      << "\",\"compatibility_semantics_contract_id\":\""
      << EscapeJsonString(summary.compatibility_semantics_contract_id)
      << "\",\"frontend_surface_path\":\""
      << EscapeJsonString(summary.frontend_surface_path)
      << "\",\"semantic_model\":\""
      << EscapeJsonString(summary.semantic_model)
      << "\",\"language_profile_model\":\""
      << EscapeJsonString(summary.language_profile_model)
      << "\",\"effective_language_profile\":\""
      << EscapeJsonString(summary.effective_language_profile)
      << "\",\"canonical_literal_rejection_diagnostics_enabled\":"
      << (summary.canonical_literal_rejection_diagnostics_enabled ? "true"
                                                                  : "false")
      << ",\"current_run_canonical_literal_rejection_sites\":"
      << summary.current_run_legacy_literal_sites
      << ",\"current_run_canonicalization_candidate_sites\":"
      << summary.current_run_canonicalization_candidate_sites
      << ",\"fixit_family_count\":" << summary.fixit_family_count
      << ",\"fail_closed\":" << (summary.fail_closed ? "true" : "false")
      << ",\"selected_configuration_valid\":"
      << (summary.selected_configuration_valid ? "true" : "false")
      << ",\"language_profile_semantics_landed\":"
      << (summary.language_profile_semantics_landed ? "true" : "false")
      << ",\"canonical_literal_rejection_semantics_landed\":"
      << (summary.canonical_literal_rejection_semantics_landed ? "true"
                                                               : "false")
      << ",\"canonical_mode_rejection_code\":\""
      << EscapeJsonString(summary.canonical_mode_rejection_code)
      << "\",\"canonical_mode_rejection_ready\":"
      << (summary.canonical_mode_rejection_ready ? "true" : "false")
      << ",\"feature_specific_fixit_surface_ready\":"
      << (summary.feature_specific_fixit_surface_ready ? "true" : "false")
      << ",\"deterministic_handoff\":"
      << (summary.deterministic_handoff ? "true" : "false")
      << ",\"ready_for_lowering_and_runtime\":"
      << (summary.ready_for_lowering_and_runtime ? "true" : "false")
      << ",\"failure_reason\":\"" << EscapeJsonString(summary.failure_reason)
      << "\",\"replay_key\":\"" << EscapeJsonString(summary.replay_key)
      << "\"}";
  return out.str();
}
