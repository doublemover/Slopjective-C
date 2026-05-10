#include "pipeline/objc3_ir_emission_core_feature_surface_readiness_publication.h"

namespace objc3_ir_emission_core_feature_surface {

void PublishObjc3IREmissionCoreFeatureQualityReadiness(
    Objc3IREmissionCoreFeatureImplementationSurface &surface,
    const Objc3ParseLoweringReadinessSurface &parse_surface) {
  surface.diagnostics_hardening_consistent =
      surface.core_feature_edge_case_robustness_ready &&
      surface.parse_artifact_diagnostics_hardening_consistent;
  surface.diagnostics_hardening_key_transport_ready =
      !surface.pass_graph_diagnostics_hardening_key.empty() &&
      !surface.parse_artifact_diagnostics_hardening_key.empty() &&
      !surface.edge_case_robustness_key.empty();
  surface.core_feature_diagnostics_hardening_ready =
      surface.core_feature_edge_case_robustness_ready &&
      surface.pass_graph_diagnostics_hardening_ready &&
      surface.diagnostics_hardening_consistent &&
      surface.diagnostics_hardening_key_transport_ready;
  surface.diagnostics_hardening_key =
      BuildObjc3IREmissionCoreFeatureDiagnosticsHardeningKey(surface);
  surface.recovery_determinism_consistent =
      surface.core_feature_diagnostics_hardening_ready &&
      surface.parse_artifact_recovery_determinism_hardening_consistent;
  surface.recovery_determinism_key_transport_ready =
      !surface.pass_graph_recovery_determinism_key.empty() &&
      !surface.parse_artifact_recovery_determinism_hardening_key.empty() &&
      !surface.diagnostics_hardening_key.empty();
  surface.core_feature_recovery_determinism_ready =
      surface.core_feature_diagnostics_hardening_ready &&
      surface.pass_graph_recovery_determinism_ready &&
      surface.recovery_determinism_consistent &&
      surface.recovery_determinism_key_transport_ready;
  surface.recovery_determinism_key =
      BuildObjc3IREmissionCoreFeatureRecoveryDeterminismHardeningKey(surface);
  surface.conformance_matrix_consistent =
      surface.core_feature_recovery_determinism_ready &&
      surface.parse_artifact_conformance_matrix_consistent &&
      surface.parse_artifact_replay_key_deterministic;
  surface.conformance_matrix_key_transport_ready =
      !surface.pass_graph_conformance_matrix_key.empty() &&
      !surface.parse_artifact_conformance_matrix_key.empty() &&
      !surface.recovery_determinism_key.empty();
  surface.core_feature_conformance_matrix_ready =
      surface.core_feature_recovery_determinism_ready &&
      surface.pass_graph_conformance_matrix_ready &&
      surface.conformance_matrix_consistent &&
      surface.conformance_matrix_key_transport_ready;
  surface.conformance_matrix_key =
      BuildObjc3IREmissionCoreFeatureConformanceMatrixKey(surface);
  surface.conformance_corpus_consistent =
      surface.core_feature_conformance_matrix_ready &&
      surface.parse_artifact_conformance_corpus_consistent &&
      surface.parse_artifact_replay_key_deterministic;
  surface.conformance_corpus_key_transport_ready =
      !surface.pass_graph_conformance_corpus_key.empty() &&
      !surface.parse_artifact_conformance_corpus_key.empty() &&
      !surface.conformance_matrix_key.empty();
  surface.core_feature_conformance_corpus_ready =
      surface.core_feature_conformance_matrix_ready &&
      surface.pass_graph_conformance_corpus_ready &&
      surface.conformance_corpus_consistent &&
      surface.conformance_corpus_key_transport_ready;
  surface.conformance_corpus_key =
      BuildObjc3IREmissionCoreFeatureConformanceCorpusKey(surface);
  surface.performance_quality_guardrails_consistent =
      surface.core_feature_conformance_corpus_ready &&
      surface.parse_artifact_performance_quality_guardrails_consistent;
  surface.performance_quality_guardrails_key_transport_ready =
      !surface.pass_graph_performance_quality_guardrails_key.empty() &&
      !surface.parse_artifact_performance_quality_guardrails_key.empty() &&
      !surface.conformance_corpus_key.empty();
  surface.core_feature_performance_quality_guardrails_ready =
      surface.core_feature_conformance_corpus_ready &&
      surface.pass_graph_performance_quality_guardrails_ready &&
      surface.performance_quality_guardrails_consistent &&
      surface.performance_quality_guardrails_key_transport_ready;
  surface.performance_quality_guardrails_key =
      BuildObjc3IREmissionCoreFeaturePerformanceQualityGuardrailsKey(surface);
  surface.cross_lane_integration_sync_consistent =
      surface.core_feature_performance_quality_guardrails_ready &&
      surface.parse_artifact_cross_lane_integration_sync_consistent &&
      parse_surface.typed_sema_cross_lane_integration_ready &&
      parse_surface.toolchain_runtime_ga_operations_cross_lane_integration_ready;
  surface.cross_lane_integration_sync_key_transport_ready =
      !surface.pass_graph_cross_lane_integration_sync_key.empty() &&
      !surface.parse_artifact_cross_lane_integration_sync_key.empty() &&
      !surface.performance_quality_guardrails_key.empty();
  surface.core_feature_cross_lane_integration_sync_ready =
      surface.core_feature_performance_quality_guardrails_ready &&
      surface.pass_graph_cross_lane_integration_sync_ready &&
      surface.cross_lane_integration_sync_consistent &&
      surface.cross_lane_integration_sync_key_transport_ready;
  surface.cross_lane_integration_sync_key =
      BuildObjc3IREmissionCoreFeatureCrossLaneIntegrationSyncKey(surface);
}

}  // namespace objc3_ir_emission_core_feature_surface
