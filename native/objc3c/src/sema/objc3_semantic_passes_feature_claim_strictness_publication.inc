static std::string BuildCompatibilityStrictnessClaimSemanticsReplayKey(
    const Objc3CompatibilityStrictnessClaimSemanticsSummary &summary) {
  std::ostringstream out;
  out << summary.contract_id
      << ";inventory_contract_id="
      << summary.runnable_feature_claim_inventory_contract_id
      << ";truth_surface_contract_id="
      << summary.feature_claim_truth_surface_contract_id
      << ";surface_path=" << summary.surface_path
      << ";semantic_model=" << summary.semantic_model
      << ";downgrade_model=" << summary.downgrade_model
      << ";rejection_model=" << summary.rejection_model
      << ";canonical_interface_truth_model="
      << summary.canonical_interface_truth_model
      << ";separate_compilation_macro_truth_model="
      << summary.separate_compilation_macro_truth_model
      << ";canonical_interface_payload_mode="
      << summary.canonical_interface_payload_mode
      << ";language_profile=" << summary.effective_language_profile
      << ";valid_language_profiles="
      << summary.valid_language_profile_count
      << ";live_selection_surfaces=" << summary.live_selection_surface_count
      << ";valid_selection_combinations="
      << summary.valid_selection_combination_count
      << ";runnable_claims=" << summary.runnable_feature_claim_count
      << ";downgraded_source_only_claims="
      << summary.downgraded_source_only_claim_count
      << ";rejected_feature_claims="
      << summary.rejected_unsupported_feature_claim_count
      << ";live_unsupported_feature_families="
      << summary.live_unsupported_feature_family_count
      << ";live_unsupported_feature_sites="
      << summary.live_unsupported_feature_site_count
      << ";live_unsupported_feature_diagnostics="
      << summary.live_unsupported_feature_diagnostic_count
      << ";throws_source_rejections="
      << summary.throws_source_rejection_site_count
      << ";blocks_source_rejections="
      << summary.blocks_source_rejection_site_count
      << ";arc_source_rejections="
      << summary.arc_source_rejection_site_count
      << ";rejected_selection_surfaces="
      << summary.rejected_selection_surface_count
      << ";suppressed_macro_claims="
      << summary.suppressed_macro_claim_count
      << ";suppressed_macro_claim_ids=";
  for (std::size_t index = 0; index < summary.suppressed_macro_claim_ids.size();
       ++index) {
    if (index > 0u) {
      out << ",";
    }
    out << summary.suppressed_macro_claim_ids[index];
  }
  return out.str();
}

static Objc3CompatibilityStrictnessClaimSemanticsSummary
BuildCompatibilityStrictnessClaimSemanticsSummaryFromIntegrationSurface(
    const Objc3SemanticIntegrationSurface &surface,
    const Objc3UnsupportedFeatureClaimEnforcementStats
        &unsupported_feature_enforcement) {
  Objc3CompatibilityStrictnessClaimSemanticsSummary summary;
  summary.effective_language_profile = "canonical";
  summary.valid_language_profile_count =
      kObjc3CompatibilityStrictnessClaimValidLanguageProfileCount;
  summary.live_selection_surface_count =
      kObjc3CompatibilityStrictnessClaimLiveSelectionSurfaceCount;
  summary.valid_selection_combination_count =
      kObjc3CompatibilityStrictnessClaimValidSelectionCombinationCount;
  summary.runnable_feature_claim_count =
      kObjc3CompatibilityStrictnessClaimRunnableFeatureCount;
  summary.downgraded_source_only_claim_count =
      kObjc3CompatibilityStrictnessClaimSourceOnlyFeatureCount;
  summary.rejected_unsupported_feature_claim_count =
      kObjc3CompatibilityStrictnessClaimRejectedFeatureCount;
  summary.rejected_selection_surface_count =
      kObjc3CompatibilityStrictnessClaimRejectedSelectionSurfaceCount;
  summary.suppressed_macro_claim_count =
      kObjc3CompatibilityStrictnessClaimSuppressedMacroClaimCount;
  summary.live_unsupported_feature_family_count =
      unsupported_feature_enforcement.live_unsupported_feature_family_count;
  summary.live_unsupported_feature_site_count =
      unsupported_feature_enforcement.live_unsupported_feature_site_count;
  summary.live_unsupported_feature_diagnostic_count =
      unsupported_feature_enforcement.live_unsupported_feature_diagnostic_count;
  summary.throws_source_rejection_site_count =
      unsupported_feature_enforcement.throws_source_rejection_site_count;
  summary.blocks_source_rejection_site_count =
      unsupported_feature_enforcement.blocks_source_rejection_site_count;
  summary.arc_source_rejection_site_count =
      unsupported_feature_enforcement.arc_source_rejection_site_count;
  summary.suppressed_macro_claim_ids = {
      kObjc3SuppressedMacroClaimStrictnessLevel,
      kObjc3SuppressedMacroClaimConcurrencyMode,
      kObjc3SuppressedMacroClaimConcurrencyStrict,
  };
  summary.fail_closed = true;
  summary.language_profile_semantics_landed = true;
  summary.canonical_literal_rejection_semantics_landed = true;
  summary.source_only_claim_downgrade_semantics_landed = true;
  summary.unsupported_feature_claim_rejection_semantics_landed = true;
  summary.live_unsupported_feature_source_rejection_landed = true;
  summary.strictness_selection_rejection_semantics_landed = true;
  summary.feature_macro_claim_suppression_semantics_landed = true;
  summary.canonical_interface_truth_semantics_landed = true;
  summary.separate_compilation_macro_truth_semantics_landed = true;
  summary.selected_configuration_valid = true;
  summary.selected_configuration_downgraded = false;
  summary.selected_configuration_rejected = false;
  // semantic truth anchor: the current native toolchain still has
  // no standalone textual-interface payload, so canonical interface truth is
  // fail-closed by explicitly publishing that absence and the suppressed macro
  // claim set any future separate-compilation surface must preserve.
  summary.ready_for_lowering_and_runtime =
      surface.interface_implementation_summary.deterministic &&
      unsupported_feature_enforcement.live_unsupported_feature_site_count == 0u &&
      unsupported_feature_enforcement.live_unsupported_feature_diagnostic_count ==
          unsupported_feature_enforcement.live_unsupported_feature_site_count;
  if (summary.ready_for_lowering_and_runtime) {
    summary.replay_key =
        BuildCompatibilityStrictnessClaimSemanticsReplayKey(summary);
  }
  if (!IsReadyObjc3CompatibilityStrictnessClaimSemanticsSummary(summary)) {
    summary.failure_reason =
        "compatibility/strictness/claim semantics summary is incomplete";
  }
  return summary;
}
