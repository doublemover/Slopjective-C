namespace {

void RecordObjc3IREmissionReadinessFailures(
    const Objc3IREmissionCoreFeatureImplementationSurface
        &ir_emission_core_feature_impl_surface,
    PostPipelineFailureRecorder &recorder) {
  std::string ir_emission_core_feature_impl_error;
  if (!IsObjc3IREmissionCoreFeatureImplementationReady(
          ir_emission_core_feature_impl_surface,
          ir_emission_core_feature_impl_error)) {
    recorder.Record(
        "O3L306",
        "LLVM IR emission failed: IR emission core feature implementation check failed: " +
            ir_emission_core_feature_impl_error);
  }

  std::string ir_emission_core_feature_expansion_error;
  if (!IsObjc3IREmissionCoreFeatureExpansionReady(
          ir_emission_core_feature_impl_surface,
          ir_emission_core_feature_expansion_error)) {
    recorder.Record(
        "O3L314",
        "LLVM IR emission failed: IR emission core feature expansion check failed: " +
            ir_emission_core_feature_expansion_error);
  }

  std::string ir_emission_core_feature_edge_case_compatibility_error;
  if (!IsObjc3IREmissionCoreFeatureEdgeCaseCompatibilityReady(
          ir_emission_core_feature_impl_surface,
          ir_emission_core_feature_edge_case_compatibility_error)) {
    recorder.Record(
        "O3L316",
        "LLVM IR emission failed: IR emission core feature edge-case compatibility check failed: " +
            ir_emission_core_feature_edge_case_compatibility_error);
  }

  std::string ir_emission_core_feature_edge_case_robustness_error;
  if (!IsObjc3IREmissionCoreFeatureEdgeCaseRobustnessReady(
          ir_emission_core_feature_impl_surface,
          ir_emission_core_feature_edge_case_robustness_error)) {
    recorder.Record(
        "O3L317",
        "LLVM IR emission failed: IR emission core feature edge-case robustness check failed: " +
            ir_emission_core_feature_edge_case_robustness_error);
  }

  std::string ir_emission_core_feature_diagnostics_hardening_error;
  if (!IsObjc3IREmissionCoreFeatureDiagnosticsHardeningReady(
          ir_emission_core_feature_impl_surface,
          ir_emission_core_feature_diagnostics_hardening_error)) {
    recorder.Record(
        "O3L318",
        "LLVM IR emission failed: IR emission core feature diagnostics hardening check failed: " +
            ir_emission_core_feature_diagnostics_hardening_error);
  }

  std::string ir_emission_core_feature_recovery_determinism_hardening_error;
  if (!IsObjc3IREmissionCoreFeatureRecoveryDeterminismHardeningReady(
          ir_emission_core_feature_impl_surface,
          ir_emission_core_feature_recovery_determinism_hardening_error)) {
    recorder.Record(
        "O3L319",
        "LLVM IR emission failed: IR emission core feature recovery determinism hardening check failed: " +
            ir_emission_core_feature_recovery_determinism_hardening_error);
  }

  std::string ir_emission_core_feature_conformance_matrix_error;
  if (!IsObjc3IREmissionCoreFeatureConformanceMatrixReady(
          ir_emission_core_feature_impl_surface,
          ir_emission_core_feature_conformance_matrix_error)) {
    recorder.Record(
        "O3L320",
        "LLVM IR emission failed: IR emission core feature conformance matrix check failed: " +
            ir_emission_core_feature_conformance_matrix_error);
  }

  std::string ir_emission_core_feature_conformance_corpus_error;
  if (!IsObjc3IREmissionCoreFeatureConformanceCorpusReady(
          ir_emission_core_feature_impl_surface,
          ir_emission_core_feature_conformance_corpus_error)) {
    recorder.Record(
        "O3L330",
        "LLVM IR emission failed: IR emission core feature conformance corpus check failed: " +
            ir_emission_core_feature_conformance_corpus_error);
  }

  std::string ir_emission_core_feature_performance_quality_guardrails_error;
  if (!IsObjc3IREmissionCoreFeaturePerformanceQualityGuardrailsReady(
          ir_emission_core_feature_impl_surface,
          ir_emission_core_feature_performance_quality_guardrails_error)) {
    recorder.Record(
        "O3L331",
        "LLVM IR emission failed: IR emission core feature performance quality guardrails check failed: " +
            ir_emission_core_feature_performance_quality_guardrails_error);
  }

  std::string ir_emission_core_feature_cross_lane_integration_sync_error;
  if (!IsObjc3IREmissionCoreFeatureCrossLaneIntegrationSyncReady(
          ir_emission_core_feature_impl_surface,
          ir_emission_core_feature_cross_lane_integration_sync_error)) {
    recorder.Record(
        "O3L332",
        "LLVM IR emission failed: IR emission core feature cross-lane integration sync check failed: " +
            ir_emission_core_feature_cross_lane_integration_sync_error);
  }

  std::string ir_emission_core_feature_advanced_core_shard1_error;
  if (!IsObjc3IREmissionCoreFeatureAdvancedCoreShard1Ready(
          ir_emission_core_feature_impl_surface,
          ir_emission_core_feature_advanced_core_shard1_error)) {
    recorder.Record(
        "O3L333",
        "LLVM IR emission failed: IR emission core feature advanced core shard 1 check failed: " +
            ir_emission_core_feature_advanced_core_shard1_error);
  }

  std::string ir_emission_core_feature_advanced_edge_compatibility_shard1_error;
  if (!IsObjc3IREmissionCoreFeatureAdvancedEdgeCompatibilityShard1Ready(
          ir_emission_core_feature_impl_surface,
          ir_emission_core_feature_advanced_edge_compatibility_shard1_error)) {
    recorder.Record(
        "O3L334",
        "LLVM IR emission failed: IR emission core feature advanced edge compatibility shard 1 check failed: " +
            ir_emission_core_feature_advanced_edge_compatibility_shard1_error);
  }

  std::string ir_emission_core_feature_advanced_diagnostics_shard1_error;
  if (!IsObjc3IREmissionCoreFeatureAdvancedDiagnosticsShard1Ready(
          ir_emission_core_feature_impl_surface,
          ir_emission_core_feature_advanced_diagnostics_shard1_error)) {
    recorder.Record(
        "O3L335",
        "LLVM IR emission failed: IR emission core feature advanced diagnostics shard 1 check failed: " +
            ir_emission_core_feature_advanced_diagnostics_shard1_error);
  }

  std::string ir_emission_core_feature_advanced_conformance_shard1_error;
  if (!IsObjc3IREmissionCoreFeatureAdvancedConformanceShard1Ready(
          ir_emission_core_feature_impl_surface,
          ir_emission_core_feature_advanced_conformance_shard1_error)) {
    recorder.Record(
        "O3L336",
        "LLVM IR emission failed: IR emission core feature advanced conformance shard 1 check failed: " +
            ir_emission_core_feature_advanced_conformance_shard1_error);
  }

  std::string ir_emission_core_feature_advanced_integration_shard1_error;
  if (!IsObjc3IREmissionCoreFeatureAdvancedIntegrationShard1Ready(
          ir_emission_core_feature_impl_surface,
          ir_emission_core_feature_advanced_integration_shard1_error)) {
    recorder.Record(
        "O3L337",
        "LLVM IR emission failed: IR emission core feature advanced integration shard 1 check failed: " +
            ir_emission_core_feature_advanced_integration_shard1_error);
  }
}

}  // namespace
