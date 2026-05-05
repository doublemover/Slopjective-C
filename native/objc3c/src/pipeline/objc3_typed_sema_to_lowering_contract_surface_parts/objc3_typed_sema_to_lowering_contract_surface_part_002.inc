  surface.typed_diagnostics_hardening_key =
      BuildObjc3TypedSemaToLoweringDiagnosticsHardeningKey(surface);
  const bool typed_diagnostics_hardening_key_ready =
      !surface.typed_diagnostics_hardening_key.empty();
  surface.typed_recovery_determinism_consistent =
      surface.typed_diagnostics_hardening_ready &&
      surface.parse_artifact_replay_key_deterministic &&
      surface.semantic_handoff_deterministic;
  surface.typed_recovery_determinism_ready =
      surface.typed_recovery_determinism_consistent &&
      !surface.typed_diagnostics_hardening_key.empty() &&
      !surface.typed_core_feature_edge_case_robustness_key.empty();
  surface.typed_recovery_determinism_key =
      BuildObjc3TypedSemaToLoweringRecoveryDeterminismKey(surface);
  const bool typed_recovery_determinism_key_ready =
      !surface.typed_recovery_determinism_key.empty();
  surface.typed_conformance_matrix_consistent =
      surface.typed_recovery_determinism_ready &&
      surface.parse_artifact_replay_key_deterministic &&
      surface.semantic_handoff_deterministic &&
      surface.sema_parity_surface_deterministic;
  surface.typed_conformance_matrix_ready =
      surface.typed_conformance_matrix_consistent &&
      !surface.typed_recovery_determinism_key.empty() &&
      !surface.typed_diagnostics_hardening_key.empty();
  surface.typed_conformance_matrix_key =
      BuildObjc3TypedSemaToLoweringConformanceMatrixKey(surface);
  const bool typed_conformance_matrix_key_ready =
      !surface.typed_conformance_matrix_key.empty();
  surface.typed_conformance_corpus_consistent =
      surface.typed_conformance_matrix_ready &&
      surface.parse_artifact_replay_key_deterministic &&
      surface.semantic_handoff_deterministic;
  surface.typed_conformance_corpus_ready =
      surface.typed_conformance_corpus_consistent &&
      !surface.typed_conformance_matrix_key.empty() &&
      !surface.typed_recovery_determinism_key.empty();
  surface.typed_conformance_corpus_key =
      BuildObjc3TypedSemaToLoweringConformanceCorpusKey(surface);
  const bool typed_conformance_corpus_key_ready =
      !surface.typed_conformance_corpus_key.empty();
  surface.typed_performance_quality_guardrails_case_count =
      kObjc3TypedSemaToLoweringPerformanceQualityGuardrailsCaseCount;
  surface.typed_performance_quality_guardrails_passed_case_count =
      static_cast<std::size_t>(surface.typed_conformance_corpus_consistent) +
      static_cast<std::size_t>(surface.typed_conformance_corpus_ready) +
      static_cast<std::size_t>(surface.parse_artifact_replay_key_deterministic) +
      static_cast<std::size_t>(surface.semantic_handoff_deterministic);
  surface.typed_performance_quality_guardrails_failed_case_count =
      surface.typed_performance_quality_guardrails_case_count >=
              surface.typed_performance_quality_guardrails_passed_case_count
          ? (surface.typed_performance_quality_guardrails_case_count -
             surface.typed_performance_quality_guardrails_passed_case_count)
          : surface.typed_performance_quality_guardrails_case_count;
  const bool typed_performance_quality_guardrails_case_accounting_consistent =
      surface.typed_performance_quality_guardrails_case_count ==
          kObjc3TypedSemaToLoweringPerformanceQualityGuardrailsCaseCount &&
      surface.typed_performance_quality_guardrails_case_count > 0 &&
      surface.typed_performance_quality_guardrails_passed_case_count <=
          surface.typed_performance_quality_guardrails_case_count &&
      surface.typed_performance_quality_guardrails_failed_case_count ==
          (surface.typed_performance_quality_guardrails_case_count -
           surface.typed_performance_quality_guardrails_passed_case_count);
  const bool typed_performance_quality_guardrails_cases_passed =
      surface.typed_performance_quality_guardrails_passed_case_count ==
          surface.typed_performance_quality_guardrails_case_count &&
      surface.typed_performance_quality_guardrails_failed_case_count == 0;
  surface.typed_performance_quality_guardrails_consistent =
      typed_performance_quality_guardrails_case_accounting_consistent &&
      typed_performance_quality_guardrails_cases_passed &&
      typed_conformance_corpus_key_ready;
  surface.typed_performance_quality_guardrails_ready =
      surface.typed_performance_quality_guardrails_consistent &&
      !surface.typed_conformance_corpus_key.empty() &&
      !surface.typed_conformance_matrix_key.empty() &&
      !surface.typed_recovery_determinism_key.empty();
  surface.typed_performance_quality_guardrails_key =
      BuildObjc3TypedSemaToLoweringPerformanceQualityGuardrailsKey(surface);
  const bool typed_performance_quality_guardrails_key_ready =
      !surface.typed_performance_quality_guardrails_key.empty();
  surface.typed_cross_lane_integration_consistent =
      surface.typed_performance_quality_guardrails_ready &&
      surface.parse_artifact_replay_key_deterministic &&
      surface.semantic_handoff_deterministic &&
      surface.typed_performance_quality_guardrails_case_count > 0;
  surface.typed_cross_lane_integration_ready =
      surface.typed_cross_lane_integration_consistent &&
      !surface.typed_performance_quality_guardrails_key.empty() &&
      !surface.typed_conformance_corpus_key.empty();
  surface.typed_cross_lane_integration_key =
      BuildObjc3TypedSemaToLoweringCrossLaneIntegrationKey(surface);
  const bool typed_cross_lane_integration_key_ready =
      !surface.typed_cross_lane_integration_key.empty();
  surface.typed_docs_runbook_sync_consistent =
      surface.typed_cross_lane_integration_ready &&
      surface.parse_artifact_replay_key_deterministic &&
      surface.semantic_handoff_deterministic;
  surface.typed_docs_runbook_sync_ready =
      surface.typed_docs_runbook_sync_consistent &&
      !surface.typed_cross_lane_integration_key.empty() &&
      !surface.typed_performance_quality_guardrails_key.empty();
  surface.typed_docs_runbook_sync_key =
      BuildObjc3TypedSemaToLoweringDocsRunbookSyncKey(surface);
  const bool typed_docs_runbook_sync_key_ready =
      !surface.typed_docs_runbook_sync_key.empty();
  surface.typed_release_candidate_replay_dry_run_consistent =
      surface.typed_docs_runbook_sync_ready &&
      surface.parse_artifact_replay_key_deterministic &&
      surface.semantic_handoff_deterministic;
  surface.typed_release_candidate_replay_dry_run_ready =
      surface.typed_release_candidate_replay_dry_run_consistent &&
      !surface.typed_docs_runbook_sync_key.empty() &&
      !surface.typed_cross_lane_integration_key.empty();
  surface.typed_release_candidate_replay_dry_run_key =
      BuildObjc3TypedSemaToLoweringReleaseCandidateReplayDryRunKey(surface);
  const bool typed_release_candidate_replay_dry_run_key_ready =
      !surface.typed_release_candidate_replay_dry_run_key.empty();
  surface.typed_advanced_core_shard1_consistent =
      surface.typed_release_candidate_replay_dry_run_ready &&
      surface.parse_artifact_replay_key_deterministic &&
      surface.semantic_handoff_deterministic;
  surface.typed_advanced_core_shard1_ready =
      surface.typed_advanced_core_shard1_consistent &&
      !surface.typed_release_candidate_replay_dry_run_key.empty() &&
      !surface.typed_docs_runbook_sync_key.empty();
  surface.typed_advanced_core_shard1_key =
      BuildObjc3TypedSemaToLoweringAdvancedCoreShard1Key(surface);
  const bool typed_advanced_core_shard1_key_ready =
      !surface.typed_advanced_core_shard1_key.empty();
  surface.typed_advanced_edge_compatibility_shard1_consistent =
      surface.typed_advanced_core_shard1_ready &&
      surface.parse_artifact_replay_key_deterministic &&
      surface.semantic_handoff_deterministic;
  surface.typed_advanced_edge_compatibility_shard1_ready =
      surface.typed_advanced_edge_compatibility_shard1_consistent &&
      !surface.typed_advanced_core_shard1_key.empty() &&
      !surface.typed_release_candidate_replay_dry_run_key.empty();
  surface.typed_advanced_edge_compatibility_shard1_key =
      BuildObjc3TypedSemaToLoweringAdvancedEdgeCompatibilityShard1Key(surface);
  const bool typed_advanced_edge_compatibility_shard1_key_ready =
      !surface.typed_advanced_edge_compatibility_shard1_key.empty();
  surface.typed_advanced_diagnostics_shard1_consistent =
      surface.typed_advanced_edge_compatibility_shard1_ready &&
      surface.parse_artifact_replay_key_deterministic &&
      surface.semantic_handoff_deterministic;
  surface.typed_advanced_diagnostics_shard1_ready =
      surface.typed_advanced_diagnostics_shard1_consistent &&
      !surface.typed_advanced_edge_compatibility_shard1_key.empty() &&
      !surface.typed_release_candidate_replay_dry_run_key.empty();
  surface.typed_advanced_diagnostics_shard1_key =
      BuildObjc3TypedSemaToLoweringAdvancedDiagnosticsShard1Key(surface);
  const bool typed_advanced_diagnostics_shard1_key_ready =
      !surface.typed_advanced_diagnostics_shard1_key.empty();
  surface.typed_advanced_conformance_shard1_consistent =
      surface.typed_advanced_diagnostics_shard1_ready &&
      surface.parse_artifact_replay_key_deterministic &&
      surface.semantic_handoff_deterministic;
  surface.typed_advanced_conformance_shard1_ready =
      surface.typed_advanced_conformance_shard1_consistent &&
      !surface.typed_advanced_diagnostics_shard1_key.empty() &&
      !surface.typed_release_candidate_replay_dry_run_key.empty();
  surface.typed_advanced_conformance_shard1_key =
      BuildObjc3TypedSemaToLoweringAdvancedConformanceShard1Key(surface);
  const bool typed_advanced_conformance_shard1_key_ready =
      !surface.typed_advanced_conformance_shard1_key.empty();
  surface.typed_advanced_integration_shard1_consistent =
      surface.typed_advanced_conformance_shard1_ready &&
      surface.parse_artifact_replay_key_deterministic &&
      surface.semantic_handoff_deterministic;
  surface.typed_advanced_integration_shard1_ready =
      surface.typed_advanced_integration_shard1_consistent &&
      !surface.typed_advanced_conformance_shard1_key.empty() &&
      !surface.typed_release_candidate_replay_dry_run_key.empty();
  surface.typed_advanced_integration_shard1_key =
      BuildObjc3TypedSemaToLoweringAdvancedIntegrationShard1Key(surface);
  const bool typed_advanced_integration_shard1_key_ready =
      !surface.typed_advanced_integration_shard1_key.empty();
  surface.typed_advanced_performance_shard1_consistent =
      surface.typed_advanced_integration_shard1_ready &&
      surface.parse_artifact_replay_key_deterministic &&
      surface.semantic_handoff_deterministic;
  surface.typed_advanced_performance_shard1_ready =
      surface.typed_advanced_performance_shard1_consistent &&
      !surface.typed_advanced_integration_shard1_key.empty() &&
      !surface.typed_release_candidate_replay_dry_run_key.empty();
  surface.typed_advanced_performance_shard1_key =
      BuildObjc3TypedSemaToLoweringAdvancedPerformanceShard1Key(surface);
  const bool typed_advanced_performance_shard1_key_ready =
      !surface.typed_advanced_performance_shard1_key.empty();
  surface.typed_advanced_core_shard2_consistent =
      surface.typed_advanced_performance_shard1_ready &&
      surface.parse_artifact_replay_key_deterministic &&
      surface.semantic_handoff_deterministic;
  surface.typed_advanced_core_shard2_ready =
      surface.typed_advanced_core_shard2_consistent &&
      !surface.typed_advanced_performance_shard1_key.empty() &&
      !surface.typed_release_candidate_replay_dry_run_key.empty();
  surface.typed_advanced_core_shard2_key =
      BuildObjc3TypedSemaToLoweringAdvancedCoreShard2Key(surface);
  const bool typed_advanced_core_shard2_key_ready =
      !surface.typed_advanced_core_shard2_key.empty();
  surface.typed_advanced_edge_compatibility_shard2_consistent =
      surface.typed_advanced_core_shard2_ready &&
      surface.parse_artifact_replay_key_deterministic &&
      surface.semantic_handoff_deterministic;
  surface.typed_advanced_edge_compatibility_shard2_ready =
      surface.typed_advanced_edge_compatibility_shard2_consistent &&
      !surface.typed_advanced_core_shard2_key.empty() &&
      !surface.typed_advanced_performance_shard1_key.empty();
  surface.typed_advanced_edge_compatibility_shard2_key =
      BuildObjc3TypedSemaToLoweringAdvancedEdgeCompatibilityShard2Key(surface);
  const bool typed_advanced_edge_compatibility_shard2_key_ready =
      !surface.typed_advanced_edge_compatibility_shard2_key.empty();
  surface.typed_advanced_diagnostics_shard2_consistent =
      surface.typed_advanced_edge_compatibility_shard2_ready &&
      surface.parse_artifact_replay_key_deterministic &&
      surface.semantic_handoff_deterministic;
  surface.typed_advanced_diagnostics_shard2_ready =
      surface.typed_advanced_diagnostics_shard2_consistent &&
      !surface.typed_advanced_edge_compatibility_shard2_key.empty() &&
      !surface.typed_advanced_core_shard2_key.empty();
  surface.typed_advanced_diagnostics_shard2_key =
      BuildObjc3TypedSemaToLoweringAdvancedDiagnosticsShard2Key(surface);
  const bool typed_advanced_diagnostics_shard2_key_ready =
      !surface.typed_advanced_diagnostics_shard2_key.empty();
  surface.typed_advanced_conformance_shard2_consistent =
      surface.typed_advanced_diagnostics_shard2_ready &&
      surface.parse_artifact_replay_key_deterministic &&
      surface.semantic_handoff_deterministic;
  surface.typed_advanced_conformance_shard2_ready =
      surface.typed_advanced_conformance_shard2_consistent &&
      !surface.typed_advanced_diagnostics_shard2_key.empty() &&
      !surface.typed_advanced_edge_compatibility_shard2_key.empty();
  surface.typed_advanced_conformance_shard2_key =
      BuildObjc3TypedSemaToLoweringAdvancedConformanceShard2Key(surface);
  const bool typed_advanced_conformance_shard2_key_ready =
      !surface.typed_advanced_conformance_shard2_key.empty();
  surface.typed_advanced_integration_shard2_consistent =
      surface.typed_advanced_conformance_shard2_ready &&
      surface.parse_artifact_replay_key_deterministic &&
      surface.semantic_handoff_deterministic;
  surface.typed_advanced_integration_shard2_ready =
      surface.typed_advanced_integration_shard2_consistent &&
      !surface.typed_advanced_conformance_shard2_key.empty() &&
      !surface.typed_advanced_diagnostics_shard2_key.empty();
  surface.typed_advanced_integration_shard2_key =
      BuildObjc3TypedSemaToLoweringAdvancedIntegrationShard2Key(surface);
  const bool typed_advanced_integration_shard2_key_ready =
      !surface.typed_advanced_integration_shard2_key.empty();
  surface.typed_integration_closeout_signoff_consistent =
      surface.typed_advanced_integration_shard2_ready &&
      surface.parse_artifact_replay_key_deterministic &&
      surface.semantic_handoff_deterministic;
  surface.typed_integration_closeout_signoff_ready =
      surface.typed_integration_closeout_signoff_consistent &&
      !surface.typed_advanced_integration_shard2_key.empty();
  surface.typed_integration_closeout_signoff_key =
      BuildObjc3TypedSemaToLoweringIntegrationCloseoutSignoffKey(surface);
  const bool typed_integration_closeout_signoff_key_ready =
      !surface.typed_integration_closeout_signoff_key.empty();
  surface.typed_core_feature_consistent =
      typed_core_feature_consistent &&
      surface.typed_core_feature_expansion_consistent &&
      typed_core_feature_expansion_key_ready &&
      surface.typed_core_feature_edge_case_compatibility_ready &&
      typed_core_feature_edge_case_compatibility_key_ready &&
      surface.typed_core_feature_edge_case_expansion_consistent &&
      surface.typed_core_feature_edge_case_robustness_ready &&
      typed_core_feature_edge_case_robustness_key_ready &&
      surface.typed_diagnostics_hardening_consistent &&
      surface.typed_diagnostics_hardening_ready &&
      typed_diagnostics_hardening_key_ready &&
      surface.typed_recovery_determinism_consistent &&
      surface.typed_recovery_determinism_ready &&
      typed_recovery_determinism_key_ready &&
      surface.typed_conformance_matrix_consistent &&
      surface.typed_conformance_matrix_ready &&
      typed_conformance_matrix_key_ready &&
      surface.typed_conformance_corpus_consistent &&
      surface.typed_conformance_corpus_ready &&
      typed_conformance_corpus_key_ready &&
      surface.typed_performance_quality_guardrails_consistent &&
      surface.typed_performance_quality_guardrails_ready &&
      typed_performance_quality_guardrails_key_ready &&
      surface.typed_cross_lane_integration_consistent &&
      surface.typed_cross_lane_integration_ready &&
      typed_cross_lane_integration_key_ready &&
      surface.typed_docs_runbook_sync_consistent &&
      surface.typed_docs_runbook_sync_ready &&
      typed_docs_runbook_sync_key_ready &&
      surface.typed_release_candidate_replay_dry_run_consistent &&
      surface.typed_release_candidate_replay_dry_run_ready &&
      typed_release_candidate_replay_dry_run_key_ready &&
      surface.typed_advanced_core_shard1_consistent &&
      surface.typed_advanced_core_shard1_ready &&
      typed_advanced_core_shard1_key_ready &&
      surface.typed_advanced_edge_compatibility_shard1_consistent &&
      surface.typed_advanced_edge_compatibility_shard1_ready &&
      typed_advanced_edge_compatibility_shard1_key_ready &&
      surface.typed_advanced_diagnostics_shard1_consistent &&
      surface.typed_advanced_diagnostics_shard1_ready &&
      typed_advanced_diagnostics_shard1_key_ready &&
      surface.typed_advanced_conformance_shard1_consistent &&
      surface.typed_advanced_conformance_shard1_ready &&
      typed_advanced_conformance_shard1_key_ready &&
      surface.typed_advanced_integration_shard1_consistent &&
      surface.typed_advanced_integration_shard1_ready &&
      typed_advanced_integration_shard1_key_ready &&
      surface.typed_advanced_performance_shard1_consistent &&
      surface.typed_advanced_performance_shard1_ready &&
      typed_advanced_performance_shard1_key_ready &&
      surface.typed_advanced_core_shard2_consistent &&
      surface.typed_advanced_core_shard2_ready &&
      typed_advanced_core_shard2_key_ready &&
      surface.typed_advanced_edge_compatibility_shard2_consistent &&
      surface.typed_advanced_edge_compatibility_shard2_ready &&
      typed_advanced_edge_compatibility_shard2_key_ready &&
      surface.typed_advanced_diagnostics_shard2_consistent &&
      surface.typed_advanced_diagnostics_shard2_ready &&
      typed_advanced_diagnostics_shard2_key_ready &&
      surface.typed_advanced_conformance_shard2_consistent &&
      surface.typed_advanced_conformance_shard2_ready &&
      typed_advanced_conformance_shard2_key_ready &&
      surface.typed_advanced_integration_shard2_consistent &&
      surface.typed_advanced_integration_shard2_ready &&
      typed_advanced_integration_shard2_key_ready &&
      surface.typed_integration_closeout_signoff_consistent &&
      surface.typed_integration_closeout_signoff_ready &&
      typed_integration_closeout_signoff_key_ready;

  surface.ready_for_lowering = surface.typed_core_feature_consistent;
  surface.typed_handoff_key = BuildObjc3TypedSemaToLoweringContractHandoffKey(surface);
  surface.typed_handoff_key_deterministic =
      surface.semantic_handoff_deterministic &&
      surface.runtime_dispatch_contract_consistent &&
      surface.lowering_boundary_ready &&
      !surface.typed_handoff_key.empty();
  surface.typed_core_feature_consistent =
      surface.typed_core_feature_consistent &&
      surface.typed_handoff_key_deterministic;
  surface.typed_core_feature_key = BuildObjc3TypedSemaToLoweringCoreFeatureKey(surface);
  surface.ready_for_lowering = surface.typed_core_feature_consistent;

  if (surface.ready_for_lowering || !surface.failure_reason.empty()) {
    return surface;
  }

  if (!surface.semantic_integration_surface_built) {
    surface.failure_reason = "semantic integration surface not built";
  } else if (!surface.semantic_type_metadata_handoff_deterministic) {
    surface.failure_reason =
        semantic_type_metadata_handoff_failure_reason.empty()
            ? "semantic type metadata handoff is not deterministic"
            : "semantic type metadata handoff is not deterministic: " +
                  semantic_type_metadata_handoff_failure_reason;
  } else if (!surface.sema_parity_surface_ready) {
    surface.failure_reason = "semantic parity surface is not ready";
  } else if (!surface.sema_parity_surface_deterministic) {
    surface.failure_reason = "semantic parity surface is not deterministic";
  } else if (!surface.protocol_category_handoff_deterministic) {
    surface.failure_reason = "protocol/category handoff is not deterministic";
  } else if (!surface.class_protocol_category_linking_handoff_deterministic) {
    surface.failure_reason = "class/protocol/category linking handoff is not deterministic";
  } else if (!surface.selector_normalization_handoff_deterministic) {
    surface.failure_reason = "selector normalization handoff is not deterministic";
  } else if (!surface.property_attribute_handoff_deterministic) {
    surface.failure_reason = "property attribute handoff is not deterministic";
  } else if (!surface.object_pointer_type_handoff_deterministic) {
    surface.failure_reason = "object pointer/nullability handoff is not deterministic";
  } else if (!surface.symbol_graph_handoff_deterministic) {
    surface.failure_reason = "symbol graph handoff is not deterministic";
  } else if (!surface.scope_resolution_handoff_deterministic) {
    surface.failure_reason = "scope resolution handoff is not deterministic";
  } else if (!surface.executable_metadata_typed_lowering_handoff_ready) {
    surface.failure_reason = "typed metadata lowering handoff is not ready";
  } else if (!surface.executable_metadata_typed_lowering_handoff_deterministic) {
    surface.failure_reason = "typed metadata lowering handoff is not deterministic";
  } else if (!surface.semantic_handoff_consistent) {
    surface.failure_reason = "semantic handoff is inconsistent";
  } else if (!surface.semantic_handoff_deterministic) {
    surface.failure_reason = "semantic handoff is not deterministic";
  } else if (!surface.runtime_dispatch_contract_consistent) {
    surface.failure_reason = "runtime dispatch contract is inconsistent";
  } else if (!surface.lowering_boundary_ready) {
    surface.failure_reason = "lowering boundary is not ready";
  } else if (!surface.typed_core_feature_expansion_consistent) {
    surface.failure_reason = "typed sema-to-lowering core feature expansion is inconsistent";
  } else if (surface.typed_core_feature_expansion_key.empty()) {
    surface.failure_reason = "typed core feature expansion key is empty";
  } else if (!surface.compatibility_handoff_consistent) {
    surface.failure_reason = "typed sema-to-lowering compatibility handoff is inconsistent";
  } else if (!surface.parse_artifact_replay_key_deterministic) {
    surface.failure_reason = "typed sema-to-lowering parse artifact replay key is not deterministic";
  } else if (!surface.language_version_pragma_coordinate_order_consistent) {
    surface.failure_reason =
        "typed sema-to-lowering language version pragma coordinate order is inconsistent";
  } else if (!surface.parse_artifact_edge_case_robustness_consistent) {
    surface.failure_reason = "typed sema-to-lowering parse artifact edge-case robustness is inconsistent";
  } else if (!surface.typed_core_feature_edge_case_compatibility_ready) {
    surface.failure_reason = "typed sema-to-lowering edge-case compatibility is not ready";
  } else if (surface.typed_core_feature_edge_case_compatibility_key.empty()) {
    surface.failure_reason = "typed sema-to-lowering edge-case compatibility key is empty";
  } else if (!surface.typed_core_feature_edge_case_expansion_consistent) {
    surface.failure_reason = "typed sema-to-lowering edge-case expansion is inconsistent";
  } else if (!surface.typed_core_feature_edge_case_robustness_ready) {
    surface.failure_reason = "typed sema-to-lowering edge-case robustness is not ready";
  } else if (surface.typed_core_feature_edge_case_robustness_key.empty()) {
    surface.failure_reason = "typed sema-to-lowering edge-case robustness key is empty";
  } else if (!surface.typed_diagnostics_hardening_consistent) {
    surface.failure_reason = "typed sema-to-lowering diagnostics hardening is inconsistent";
  } else if (!surface.typed_diagnostics_hardening_ready) {
    surface.failure_reason = "typed sema-to-lowering diagnostics hardening is not ready";
  } else if (surface.typed_diagnostics_hardening_key.empty()) {
    surface.failure_reason = "typed sema-to-lowering diagnostics hardening key is empty";
  } else if (!surface.typed_recovery_determinism_consistent) {
    surface.failure_reason = "typed sema-to-lowering recovery/determinism is inconsistent";
  } else if (!surface.typed_recovery_determinism_ready) {
    surface.failure_reason = "typed sema-to-lowering recovery/determinism is not ready";
  } else if (surface.typed_recovery_determinism_key.empty()) {
    surface.failure_reason = "typed sema-to-lowering recovery/determinism key is empty";
  } else if (!surface.typed_conformance_matrix_consistent) {
    surface.failure_reason = "typed sema-to-lowering conformance matrix is inconsistent";
  } else if (!surface.typed_conformance_matrix_ready) {
    surface.failure_reason = "typed sema-to-lowering conformance matrix is not ready";
  } else if (surface.typed_conformance_matrix_key.empty()) {
    surface.failure_reason = "typed sema-to-lowering conformance matrix key is empty";
  } else if (!surface.typed_conformance_corpus_consistent) {
    surface.failure_reason = "typed sema-to-lowering conformance corpus is inconsistent";
  } else if (!surface.typed_conformance_corpus_ready) {
    surface.failure_reason = "typed sema-to-lowering conformance corpus is not ready";
  } else if (surface.typed_conformance_corpus_key.empty()) {
    surface.failure_reason = "typed sema-to-lowering conformance corpus key is empty";
  } else if (!surface.typed_performance_quality_guardrails_consistent) {
    surface.failure_reason = "typed sema-to-lowering performance/quality guardrails are inconsistent";
  } else if (!surface.typed_performance_quality_guardrails_ready) {
    surface.failure_reason = "typed sema-to-lowering performance/quality guardrails are not ready";
  } else if (surface.typed_performance_quality_guardrails_key.empty()) {
    surface.failure_reason = "typed sema-to-lowering performance/quality guardrails key is empty";
  } else if (!surface.typed_cross_lane_integration_consistent) {
    surface.failure_reason = "typed sema-to-lowering cross-lane integration is inconsistent";
  } else if (!surface.typed_cross_lane_integration_ready) {
    surface.failure_reason = "typed sema-to-lowering cross-lane integration is not ready";
  } else if (surface.typed_cross_lane_integration_key.empty()) {
    surface.failure_reason = "typed sema-to-lowering cross-lane integration key is empty";
  } else if (!surface.typed_docs_runbook_sync_consistent) {
    surface.failure_reason = "typed sema-to-lowering docs/runbook synchronization is inconsistent";
  } else if (!surface.typed_docs_runbook_sync_ready) {
    surface.failure_reason = "typed sema-to-lowering docs/runbook synchronization is not ready";
  } else if (surface.typed_docs_runbook_sync_key.empty()) {
    surface.failure_reason = "typed sema-to-lowering docs/runbook synchronization key is empty";
  } else if (!surface.typed_release_candidate_replay_dry_run_consistent) {
    surface.failure_reason =
        "typed sema-to-lowering release-candidate replay dry-run is inconsistent";
  } else if (!surface.typed_release_candidate_replay_dry_run_ready) {
    surface.failure_reason =
        "typed sema-to-lowering release-candidate replay dry-run is not ready";
  } else if (surface.typed_release_candidate_replay_dry_run_key.empty()) {
    surface.failure_reason =
        "typed sema-to-lowering release-candidate replay dry-run key is empty";
  } else if (!surface.typed_advanced_core_shard1_consistent) {
    surface.failure_reason = "typed sema-to-lowering advanced core shard 1 is inconsistent";
  } else if (!surface.typed_advanced_core_shard1_ready) {
    surface.failure_reason = "typed sema-to-lowering advanced core shard 1 is not ready";
  } else if (surface.typed_advanced_core_shard1_key.empty()) {
    surface.failure_reason = "typed sema-to-lowering advanced core shard 1 key is empty";
  } else if (!surface.typed_advanced_edge_compatibility_shard1_consistent) {
    surface.failure_reason =
        "typed sema-to-lowering advanced edge compatibility shard 1 is inconsistent";
  } else if (!surface.typed_advanced_edge_compatibility_shard1_ready) {
    surface.failure_reason =
        "typed sema-to-lowering advanced edge compatibility shard 1 is not ready";
  } else if (surface.typed_advanced_edge_compatibility_shard1_key.empty()) {
    surface.failure_reason =
        "typed sema-to-lowering advanced edge compatibility shard 1 key is empty";
  } else if (!surface.typed_advanced_diagnostics_shard1_consistent) {
    surface.failure_reason = "typed sema-to-lowering advanced diagnostics shard 1 is inconsistent";
  } else if (!surface.typed_advanced_diagnostics_shard1_ready) {
    surface.failure_reason = "typed sema-to-lowering advanced diagnostics shard 1 is not ready";
  } else if (surface.typed_advanced_diagnostics_shard1_key.empty()) {
    surface.failure_reason = "typed sema-to-lowering advanced diagnostics shard 1 key is empty";
  } else if (!surface.typed_advanced_conformance_shard1_consistent) {
    surface.failure_reason = "typed sema-to-lowering advanced conformance shard 1 is inconsistent";
  } else if (!surface.typed_advanced_conformance_shard1_ready) {
    surface.failure_reason = "typed sema-to-lowering advanced conformance shard 1 is not ready";
  } else if (surface.typed_advanced_conformance_shard1_key.empty()) {
    surface.failure_reason = "typed sema-to-lowering advanced conformance shard 1 key is empty";
  } else if (!surface.typed_advanced_integration_shard1_consistent) {
    surface.failure_reason = "typed sema-to-lowering advanced integration shard 1 is inconsistent";
  } else if (!surface.typed_advanced_integration_shard1_ready) {
    surface.failure_reason = "typed sema-to-lowering advanced integration shard 1 is not ready";
  } else if (surface.typed_advanced_integration_shard1_key.empty()) {
    surface.failure_reason = "typed sema-to-lowering advanced integration shard 1 key is empty";
  } else if (!surface.typed_advanced_performance_shard1_consistent) {
    surface.failure_reason = "typed sema-to-lowering advanced performance shard 1 is inconsistent";
  } else if (!surface.typed_advanced_performance_shard1_ready) {
    surface.failure_reason = "typed sema-to-lowering advanced performance shard 1 is not ready";
  } else if (surface.typed_advanced_performance_shard1_key.empty()) {
    surface.failure_reason = "typed sema-to-lowering advanced performance shard 1 key is empty";
  } else if (!surface.typed_advanced_core_shard2_consistent) {
    surface.failure_reason = "typed sema-to-lowering advanced core shard 2 is inconsistent";
  } else if (!surface.typed_advanced_core_shard2_ready) {
    surface.failure_reason = "typed sema-to-lowering advanced core shard 2 is not ready";
  } else if (surface.typed_advanced_core_shard2_key.empty()) {
    surface.failure_reason = "typed sema-to-lowering advanced core shard 2 key is empty";
  } else if (!surface.typed_advanced_edge_compatibility_shard2_consistent) {
    surface.failure_reason =
        "typed sema-to-lowering advanced edge compatibility shard 2 is inconsistent";
  } else if (!surface.typed_advanced_edge_compatibility_shard2_ready) {
    surface.failure_reason =
        "typed sema-to-lowering advanced edge compatibility shard 2 is not ready";
  } else if (surface.typed_advanced_edge_compatibility_shard2_key.empty()) {
    surface.failure_reason =
        "typed sema-to-lowering advanced edge compatibility shard 2 key is empty";
  } else if (!surface.typed_advanced_diagnostics_shard2_consistent) {
    surface.failure_reason = "typed sema-to-lowering advanced diagnostics shard 2 is inconsistent";
  } else if (!surface.typed_advanced_diagnostics_shard2_ready) {
    surface.failure_reason = "typed sema-to-lowering advanced diagnostics shard 2 is not ready";
  } else if (surface.typed_advanced_diagnostics_shard2_key.empty()) {
    surface.failure_reason = "typed sema-to-lowering advanced diagnostics shard 2 key is empty";
  } else if (!surface.typed_advanced_conformance_shard2_consistent) {
    surface.failure_reason = "typed sema-to-lowering advanced conformance shard 2 is inconsistent";
  } else if (!surface.typed_advanced_conformance_shard2_ready) {
    surface.failure_reason = "typed sema-to-lowering advanced conformance shard 2 is not ready";
  } else if (surface.typed_advanced_conformance_shard2_key.empty()) {
    surface.failure_reason = "typed sema-to-lowering advanced conformance shard 2 key is empty";
  } else if (!surface.typed_advanced_integration_shard2_consistent) {
    surface.failure_reason = "typed sema-to-lowering advanced integration shard 2 is inconsistent";
  } else if (!surface.typed_advanced_integration_shard2_ready) {
    surface.failure_reason = "typed sema-to-lowering advanced integration shard 2 is not ready";
  } else if (surface.typed_advanced_integration_shard2_key.empty()) {
    surface.failure_reason = "typed sema-to-lowering advanced integration shard 2 key is empty";
  } else if (!surface.typed_integration_closeout_signoff_consistent) {
    surface.failure_reason = "typed sema-to-lowering integration closeout/sign-off is inconsistent";
  } else if (!surface.typed_integration_closeout_signoff_ready) {
    surface.failure_reason = "typed sema-to-lowering integration closeout/sign-off is not ready";
  } else if (surface.typed_integration_closeout_signoff_key.empty()) {
    surface.failure_reason = "typed sema-to-lowering integration closeout/sign-off key is empty";
  } else if (!surface.typed_handoff_key_deterministic) {
    surface.failure_reason = "typed handoff key is not deterministic";
  } else if (!surface.typed_core_feature_consistent) {
    surface.failure_reason = "typed sema-to-lowering core feature contract is inconsistent";
  } else {
    surface.failure_reason = "typed sema-to-lowering contract readiness failed";
  }

  return surface;
}

inline bool IsObjc3TypedSemaToLoweringContractSurfaceReady(
    const Objc3TypedSemaToLoweringContractSurface &surface,
    std::string &failure_reason) {
  if (surface.ready_for_lowering) {
    failure_reason.clear();
    return true;
  }
  failure_reason = surface.failure_reason.empty() ? "typed sema-to-lowering readiness failed"
                                                  : surface.failure_reason;
  return false;
}
