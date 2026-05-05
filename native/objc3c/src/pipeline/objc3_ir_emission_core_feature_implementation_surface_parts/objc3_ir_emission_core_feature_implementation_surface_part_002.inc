    return true;
  }
  reason =
      surface.expansion_failure_reason.empty()
          ? "IR emission core feature expansion surface is not ready"
          : surface.expansion_failure_reason;
  return false;
}

inline bool IsObjc3IREmissionCoreFeatureEdgeCaseCompatibilityReady(
    const Objc3IREmissionCoreFeatureImplementationSurface &surface,
    std::string &reason) {
  if (surface.core_feature_edge_case_compatibility_ready &&
      surface.edge_case_compatibility_key_transport_ready &&
      !surface.edge_case_compatibility_key.empty()) {
    reason.clear();
    return true;
  }
  reason =
      surface.edge_case_compatibility_failure_reason.empty()
          ? "IR emission core feature edge-case compatibility surface is not ready"
          : surface.edge_case_compatibility_failure_reason;
  return false;
}

inline bool IsObjc3IREmissionCoreFeatureEdgeCaseRobustnessReady(
    const Objc3IREmissionCoreFeatureImplementationSurface &surface,
    std::string &reason) {
  if (surface.core_feature_edge_case_robustness_ready &&
      surface.edge_case_robustness_key_transport_ready &&
      !surface.edge_case_robustness_key.empty()) {
    reason.clear();
    return true;
  }
  reason = surface.edge_case_robustness_failure_reason.empty()
               ? "IR emission core feature edge-case robustness surface is not ready"
               : surface.edge_case_robustness_failure_reason;
  return false;
}

inline bool IsObjc3IREmissionCoreFeatureDiagnosticsHardeningReady(
    const Objc3IREmissionCoreFeatureImplementationSurface &surface,
    std::string &reason) {
  if (surface.core_feature_diagnostics_hardening_ready &&
      surface.diagnostics_hardening_key_transport_ready &&
      !surface.diagnostics_hardening_key.empty()) {
    reason.clear();
    return true;
  }
  reason =
      surface.diagnostics_hardening_failure_reason.empty()
          ? "IR emission core feature diagnostics hardening surface is not ready"
          : surface.diagnostics_hardening_failure_reason;
  return false;
}

inline bool IsObjc3IREmissionCoreFeatureRecoveryDeterminismHardeningReady(
    const Objc3IREmissionCoreFeatureImplementationSurface &surface,
    std::string &reason) {
  if (surface.core_feature_recovery_determinism_ready &&
      surface.recovery_determinism_key_transport_ready &&
      !surface.recovery_determinism_key.empty()) {
    reason.clear();
    return true;
  }
  reason =
      surface.recovery_determinism_failure_reason.empty()
          ? "IR emission core feature recovery determinism hardening surface is not ready"
          : surface.recovery_determinism_failure_reason;
  return false;
}

inline bool IsObjc3IREmissionCoreFeatureConformanceMatrixReady(
    const Objc3IREmissionCoreFeatureImplementationSurface &surface,
    std::string &reason) {
  if (surface.core_feature_conformance_matrix_ready &&
      surface.conformance_matrix_key_transport_ready &&
      !surface.conformance_matrix_key.empty()) {
    reason.clear();
    return true;
  }
  reason =
      surface.conformance_matrix_failure_reason.empty()
          ? "IR emission core feature conformance matrix surface is not ready"
          : surface.conformance_matrix_failure_reason;
  return false;
}

inline bool IsObjc3IREmissionCoreFeatureConformanceCorpusReady(
    const Objc3IREmissionCoreFeatureImplementationSurface &surface,
    std::string &reason) {
  if (surface.core_feature_conformance_corpus_ready &&
      surface.conformance_corpus_key_transport_ready &&
      !surface.conformance_corpus_key.empty()) {
    reason.clear();
    return true;
  }
  reason =
      surface.conformance_corpus_failure_reason.empty()
          ? "IR emission core feature conformance corpus surface is not ready"
          : surface.conformance_corpus_failure_reason;
  return false;
}

inline bool IsObjc3IREmissionCoreFeaturePerformanceQualityGuardrailsReady(
    const Objc3IREmissionCoreFeatureImplementationSurface &surface,
    std::string &reason) {
  if (surface.core_feature_performance_quality_guardrails_ready &&
      surface.performance_quality_guardrails_key_transport_ready &&
      !surface.performance_quality_guardrails_key.empty()) {
    reason.clear();
    return true;
  }
  reason =
      surface.performance_quality_guardrails_failure_reason.empty()
          ? "IR emission core feature performance quality guardrails surface is not ready"
          : surface.performance_quality_guardrails_failure_reason;
  return false;
}

inline bool IsObjc3IREmissionCoreFeatureCrossLaneIntegrationSyncReady(
    const Objc3IREmissionCoreFeatureImplementationSurface &surface,
    std::string &reason) {
  if (surface.core_feature_cross_lane_integration_sync_ready &&
      surface.cross_lane_integration_sync_key_transport_ready &&
      !surface.cross_lane_integration_sync_key.empty()) {
    reason.clear();
    return true;
  }
  reason = surface.cross_lane_integration_sync_failure_reason.empty()
               ? "IR emission core feature cross-lane integration sync surface is not ready"
               : surface.cross_lane_integration_sync_failure_reason;
  return false;
}

inline bool IsObjc3IREmissionCoreFeatureAdvancedCoreShard1Ready(
    const Objc3IREmissionCoreFeatureImplementationSurface &surface,
    std::string &reason) {
  if (surface.core_feature_advanced_core_shard1_ready &&
      surface.advanced_core_shard1_key_transport_ready &&
      !surface.advanced_core_shard1_key.empty()) {
    reason.clear();
    return true;
  }
  reason = surface.advanced_core_shard1_failure_reason.empty()
               ? "IR emission core feature advanced core shard 1 surface is not ready"
               : surface.advanced_core_shard1_failure_reason;
  return false;
}

inline bool IsObjc3IREmissionCoreFeatureAdvancedEdgeCompatibilityShard1Ready(
    const Objc3IREmissionCoreFeatureImplementationSurface &surface,
    std::string &reason) {
  if (surface.core_feature_advanced_edge_compatibility_shard1_ready &&
      surface.advanced_edge_compatibility_shard1_key_transport_ready &&
      !surface.advanced_edge_compatibility_shard1_key.empty()) {
    reason.clear();
    return true;
  }
  reason = surface.advanced_edge_compatibility_shard1_failure_reason.empty()
               ? "IR emission core feature advanced edge compatibility shard 1 surface is not ready"
               : surface.advanced_edge_compatibility_shard1_failure_reason;
  return false;
}

inline bool IsObjc3IREmissionCoreFeatureAdvancedDiagnosticsShard1Ready(
    const Objc3IREmissionCoreFeatureImplementationSurface &surface,
    std::string &reason) {
  if (surface.core_feature_advanced_diagnostics_shard1_ready &&
      surface.advanced_diagnostics_shard1_key_transport_ready &&
      !surface.advanced_diagnostics_shard1_key.empty()) {
    reason.clear();
    return true;
  }
  reason = surface.advanced_diagnostics_shard1_failure_reason.empty()
               ? "IR emission core feature advanced diagnostics shard 1 surface is not ready"
               : surface.advanced_diagnostics_shard1_failure_reason;
  return false;
}

inline bool IsObjc3IREmissionCoreFeatureAdvancedConformanceShard1Ready(
    const Objc3IREmissionCoreFeatureImplementationSurface &surface,
    std::string &reason) {
  if (surface.core_feature_advanced_conformance_shard1_ready &&
      surface.advanced_conformance_shard1_key_transport_ready &&
      !surface.advanced_conformance_shard1_key.empty()) {
    reason.clear();
    return true;
  }
  reason = surface.advanced_conformance_shard1_failure_reason.empty()
               ? "IR emission core feature advanced conformance shard 1 surface is not ready"
               : surface.advanced_conformance_shard1_failure_reason;
  return false;
}

inline bool IsObjc3IREmissionCoreFeatureAdvancedIntegrationShard1Ready(
    const Objc3IREmissionCoreFeatureImplementationSurface &surface,
    std::string &reason) {
  if (surface.core_feature_advanced_integration_shard1_ready &&
      surface.advanced_integration_shard1_key_transport_ready &&
      !surface.advanced_integration_shard1_key.empty()) {
    reason.clear();
    return true;
  }
  reason = surface.advanced_integration_shard1_failure_reason.empty()
               ? "IR emission core feature advanced integration shard 1 surface is not ready"
               : surface.advanced_integration_shard1_failure_reason;
  return false;
}
