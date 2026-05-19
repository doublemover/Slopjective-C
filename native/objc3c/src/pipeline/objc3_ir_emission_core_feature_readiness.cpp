#include "pipeline/objc3_ir_emission_core_feature_implementation_surface.h"

namespace {

bool ResolveObjc3IREmissionCoreFeatureReadiness(
    bool ready,
    const std::string &failure_reason,
    const char *retired_route_reason,
    std::string &reason) {
  if (ready) {
    reason.clear();
    return true;
  }
  reason = failure_reason.empty() ? retired_route_reason : failure_reason;
  return false;
}

}  // namespace

bool IsObjc3IREmissionCoreFeatureImplementationReady(
    const Objc3IREmissionCoreFeatureImplementationSurface &surface,
    std::string &reason) {
  return ResolveObjc3IREmissionCoreFeatureReadiness(
      surface.core_feature_impl_ready,
      surface.failure_reason,
      "IR emission core feature implementation surface is not ready",
      reason);
}

bool IsObjc3IREmissionCoreFeatureExpansionReady(
    const Objc3IREmissionCoreFeatureImplementationSurface &surface,
    std::string &reason) {
  return ResolveObjc3IREmissionCoreFeatureReadiness(
      surface.core_feature_expansion_ready,
      surface.expansion_failure_reason,
      "IR emission core feature expansion surface is not ready",
      reason);
}

bool IsObjc3IREmissionCoreFeatureEdgeCaseCompatibilityReady(
    const Objc3IREmissionCoreFeatureImplementationSurface &surface,
    std::string &reason) {
  return ResolveObjc3IREmissionCoreFeatureReadiness(
      surface.core_feature_edge_case_compatibility_ready &&
          surface.edge_case_compatibility_key_transport_ready &&
          !surface.edge_case_compatibility_key.empty(),
      surface.edge_case_compatibility_failure_reason,
      "IR emission core feature edge-case compatibility surface is not ready",
      reason);
}

bool IsObjc3IREmissionCoreFeatureEdgeCaseRobustnessReady(
    const Objc3IREmissionCoreFeatureImplementationSurface &surface,
    std::string &reason) {
  return ResolveObjc3IREmissionCoreFeatureReadiness(
      surface.core_feature_edge_case_robustness_ready &&
          surface.edge_case_robustness_key_transport_ready &&
          !surface.edge_case_robustness_key.empty(),
      surface.edge_case_robustness_failure_reason,
      "IR emission core feature edge-case robustness surface is not ready",
      reason);
}

bool IsObjc3IREmissionCoreFeatureDiagnosticsHardeningReady(
    const Objc3IREmissionCoreFeatureImplementationSurface &surface,
    std::string &reason) {
  return ResolveObjc3IREmissionCoreFeatureReadiness(
      surface.core_feature_diagnostics_hardening_ready &&
          surface.diagnostics_hardening_key_transport_ready &&
          !surface.diagnostics_hardening_key.empty(),
      surface.diagnostics_hardening_failure_reason,
      "IR emission core feature diagnostics hardening surface is not ready",
      reason);
}

bool IsObjc3IREmissionCoreFeatureRecoveryDeterminismHardeningReady(
    const Objc3IREmissionCoreFeatureImplementationSurface &surface,
    std::string &reason) {
  return ResolveObjc3IREmissionCoreFeatureReadiness(
      surface.core_feature_recovery_determinism_ready &&
          surface.recovery_determinism_key_transport_ready &&
          !surface.recovery_determinism_key.empty(),
      surface.recovery_determinism_failure_reason,
      "IR emission core feature recovery determinism hardening surface is not ready",
      reason);
}

bool IsObjc3IREmissionCoreFeatureConformanceMatrixReady(
    const Objc3IREmissionCoreFeatureImplementationSurface &surface,
    std::string &reason) {
  return ResolveObjc3IREmissionCoreFeatureReadiness(
      surface.core_feature_conformance_matrix_ready &&
          surface.conformance_matrix_key_transport_ready &&
          !surface.conformance_matrix_key.empty(),
      surface.conformance_matrix_failure_reason,
      "IR emission core feature conformance matrix surface is not ready",
      reason);
}

bool IsObjc3IREmissionCoreFeatureConformanceCorpusReady(
    const Objc3IREmissionCoreFeatureImplementationSurface &surface,
    std::string &reason) {
  return ResolveObjc3IREmissionCoreFeatureReadiness(
      surface.core_feature_conformance_corpus_ready &&
          surface.conformance_corpus_key_transport_ready &&
          !surface.conformance_corpus_key.empty(),
      surface.conformance_corpus_failure_reason,
      "IR emission core feature conformance corpus surface is not ready",
      reason);
}

bool IsObjc3IREmissionCoreFeaturePerformanceQualityGuardrailsReady(
    const Objc3IREmissionCoreFeatureImplementationSurface &surface,
    std::string &reason) {
  return ResolveObjc3IREmissionCoreFeatureReadiness(
      surface.core_feature_performance_quality_guardrails_ready &&
          surface.performance_quality_guardrails_key_transport_ready &&
          !surface.performance_quality_guardrails_key.empty(),
      surface.performance_quality_guardrails_failure_reason,
      "IR emission core feature performance quality guardrails surface is not ready",
      reason);
}

bool IsObjc3IREmissionCoreFeatureCrossLaneIntegrationSyncReady(
    const Objc3IREmissionCoreFeatureImplementationSurface &surface,
    std::string &reason) {
  return ResolveObjc3IREmissionCoreFeatureReadiness(
      surface.core_feature_cross_lane_integration_sync_ready &&
          surface.cross_lane_integration_sync_key_transport_ready &&
          !surface.cross_lane_integration_sync_key.empty(),
      surface.cross_lane_integration_sync_failure_reason,
      "IR emission core feature cross-lane integration sync surface is not ready",
      reason);
}

bool IsObjc3IREmissionCoreFeatureAdvancedCoreShard1Ready(
    const Objc3IREmissionCoreFeatureImplementationSurface &surface,
    std::string &reason) {
  return ResolveObjc3IREmissionCoreFeatureReadiness(
      surface.core_feature_advanced_core_shard1_ready &&
          surface.advanced_core_shard1_key_transport_ready &&
          !surface.advanced_core_shard1_key.empty(),
      surface.advanced_core_shard1_failure_reason,
      "IR emission core feature advanced core shard 1 surface is not ready",
      reason);
}

bool IsObjc3IREmissionCoreFeatureAdvancedEdgeCompatibilityShard1Ready(
    const Objc3IREmissionCoreFeatureImplementationSurface &surface,
    std::string &reason) {
  return ResolveObjc3IREmissionCoreFeatureReadiness(
      surface.core_feature_advanced_edge_compatibility_shard1_ready &&
          surface.advanced_edge_compatibility_shard1_key_transport_ready &&
          !surface.advanced_edge_compatibility_shard1_key.empty(),
      surface.advanced_edge_compatibility_shard1_failure_reason,
      "IR emission core feature advanced edge compatibility shard 1 surface is not ready",
      reason);
}

bool IsObjc3IREmissionCoreFeatureAdvancedDiagnosticsShard1Ready(
    const Objc3IREmissionCoreFeatureImplementationSurface &surface,
    std::string &reason) {
  return ResolveObjc3IREmissionCoreFeatureReadiness(
      surface.core_feature_advanced_diagnostics_shard1_ready &&
          surface.advanced_diagnostics_shard1_key_transport_ready &&
          !surface.advanced_diagnostics_shard1_key.empty(),
      surface.advanced_diagnostics_shard1_failure_reason,
      "IR emission core feature advanced diagnostics shard 1 surface is not ready",
      reason);
}

bool IsObjc3IREmissionCoreFeatureAdvancedConformanceShard1Ready(
    const Objc3IREmissionCoreFeatureImplementationSurface &surface,
    std::string &reason) {
  return ResolveObjc3IREmissionCoreFeatureReadiness(
      surface.core_feature_advanced_conformance_shard1_ready &&
          surface.advanced_conformance_shard1_key_transport_ready &&
          !surface.advanced_conformance_shard1_key.empty(),
      surface.advanced_conformance_shard1_failure_reason,
      "IR emission core feature advanced conformance shard 1 surface is not ready",
      reason);
}

bool IsObjc3IREmissionCoreFeatureAdvancedIntegrationShard1Ready(
    const Objc3IREmissionCoreFeatureImplementationSurface &surface,
    std::string &reason) {
  return ResolveObjc3IREmissionCoreFeatureReadiness(
      surface.core_feature_advanced_integration_shard1_ready &&
          surface.advanced_integration_shard1_key_transport_ready &&
          !surface.advanced_integration_shard1_key.empty(),
      surface.advanced_integration_shard1_failure_reason,
      "IR emission core feature advanced integration shard 1 surface is not ready",
      reason);
}
