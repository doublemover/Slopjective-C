#include "pipeline/objc3_ir_emission_core_feature_implementation_surface.h"

#include <sstream>

std::string BuildObjc3IREmissionCoreFeatureImplementationKey(
    const Objc3IREmissionCoreFeatureImplementationSurface &surface) {
  std::ostringstream key;
  key << "ir-emission-core-feature-implementation:v1:"
      << "modular-split-ready=" << (surface.modular_split_ready ? "true" : "false")
      << ";metadata-transport-ready="
      << (surface.metadata_transport_ready ? "true" : "false")
      << ";pass-graph-core-feature-ready="
      << (surface.pass_graph_core_feature_ready ? "true" : "false")
      << ";runtime-boundary-handoff-ready="
      << (surface.runtime_boundary_handoff_ready ? "true" : "false")
      << ";direct-ir-entrypoint-ready="
      << (surface.direct_ir_entrypoint_ready ? "true" : "false")
      << ";core-feature-impl-ready="
      << (surface.core_feature_impl_ready ? "true" : "false")
      << ";scaffold-key=" << surface.scaffold_key;
  return key.str();
}

std::string BuildObjc3IREmissionCoreFeatureExpansionKey(
    const Objc3IREmissionCoreFeatureImplementationSurface &surface) {
  std::ostringstream key;
  key << "ir-emission-core-feature-expansion:v1:"
      << "core-feature-impl-ready="
      << (surface.core_feature_impl_ready ? "true" : "false")
      << ";pass-graph-expansion-ready="
      << (surface.pass_graph_expansion_ready ? "true" : "false")
      << ";runtime-boundary-handoff-ready="
      << (surface.runtime_boundary_handoff_ready ? "true" : "false")
      << ";direct-ir-entrypoint-ready="
      << (surface.direct_ir_entrypoint_ready ? "true" : "false")
      << ";expansion-metadata-transport-ready="
      << (surface.expansion_metadata_transport_ready ? "true" : "false")
      << ";core-feature-expansion-ready="
      << (surface.core_feature_expansion_ready ? "true" : "false")
      << ";core-feature-key=" << surface.core_feature_key;
  return key.str();
}

std::string BuildObjc3IREmissionCoreFeatureEdgeCaseCompatibilityKey(
    const Objc3IREmissionCoreFeatureImplementationSurface &surface) {
  std::ostringstream key;
  key << "ir-emission-core-feature-edge-case-compatibility:v1:"
      << "core-feature-expansion-ready="
      << (surface.core_feature_expansion_ready ? "true" : "false")
      << ";pass-graph-edge-case-compatibility-ready="
      << (surface.pass_graph_edge_case_compatibility_ready ? "true" : "false")
      << ";compatibility-handoff-consistent="
      << (surface.compatibility_handoff_consistent ? "true" : "false")
      << ";language-version-pragma-coordinate-order-consistent="
      << (surface.language_version_pragma_coordinate_order_consistent ? "true"
                                                                      : "false")
      << ";parse-artifact-edge-case-robustness-consistent="
      << (surface.parse_artifact_edge_case_robustness_consistent ? "true"
                                                                 : "false")
      << ";parse-artifact-replay-key-deterministic="
      << (surface.parse_artifact_replay_key_deterministic ? "true" : "false")
      << ";edge-case-compatibility-key-transport-ready="
      << (surface.edge_case_compatibility_key_transport_ready ? "true" : "false")
      << ";edge-case-compatibility-ready="
      << (surface.core_feature_edge_case_compatibility_ready ? "true" : "false")
      << ";pass-graph-edge-case-compatibility-key="
      << surface.pass_graph_edge_case_compatibility_key
      << ";compatibility-handoff-key=" << surface.compatibility_handoff_key
      << ";parse-artifact-edge-robustness-key="
      << surface.parse_artifact_edge_robustness_key
      << ";expansion-key=" << surface.expansion_key;
  return key.str();
}

std::string BuildObjc3IREmissionCoreFeatureEdgeCaseRobustnessKey(
    const Objc3IREmissionCoreFeatureImplementationSurface &surface) {
  std::ostringstream key;
  key << "ir-emission-core-feature-edge-case-robustness:v1:"
      << "edge-case-compatibility-ready="
      << (surface.core_feature_edge_case_compatibility_ready ? "true" : "false")
      << ";pass-graph-edge-case-robustness-ready="
      << (surface.pass_graph_edge_case_robustness_ready ? "true" : "false")
      << ";edge-case-expansion-consistent="
      << (surface.edge_case_expansion_consistent ? "true" : "false")
      << ";parse-artifact-edge-case-robustness-ready="
      << (surface.parse_artifact_edge_case_robustness_ready ? "true" : "false")
      << ";edge-case-robustness-key-transport-ready="
      << (surface.edge_case_robustness_key_transport_ready ? "true" : "false")
      << ";edge-case-robustness-ready="
      << (surface.core_feature_edge_case_robustness_ready ? "true" : "false")
      << ";pass-graph-edge-case-robustness-key="
      << surface.pass_graph_edge_case_robustness_key
      << ";parse-artifact-edge-case-expansion-key="
      << surface.parse_artifact_edge_case_expansion_key
      << ";parse-artifact-edge-robustness-key="
      << surface.parse_artifact_edge_robustness_key
      << ";edge-case-compatibility-key=" << surface.edge_case_compatibility_key;
  return key.str();
}

std::string BuildObjc3IREmissionCoreFeatureDiagnosticsHardeningKey(
    const Objc3IREmissionCoreFeatureImplementationSurface &surface) {
  std::ostringstream key;
  key << "ir-emission-core-feature-diagnostics-hardening:v1:"
      << "edge-case-robustness-ready="
      << (surface.core_feature_edge_case_robustness_ready ? "true" : "false")
      << ";pass-graph-diagnostics-hardening-ready="
      << (surface.pass_graph_diagnostics_hardening_ready ? "true" : "false")
      << ";diagnostics-hardening-consistent="
      << (surface.diagnostics_hardening_consistent ? "true" : "false")
      << ";parse-artifact-diagnostics-hardening-consistent="
      << (surface.parse_artifact_diagnostics_hardening_consistent ? "true"
                                                                  : "false")
      << ";diagnostics-hardening-key-transport-ready="
      << (surface.diagnostics_hardening_key_transport_ready ? "true" : "false")
      << ";diagnostics-hardening-ready="
      << (surface.core_feature_diagnostics_hardening_ready ? "true" : "false")
      << ";pass-graph-diagnostics-hardening-key="
      << surface.pass_graph_diagnostics_hardening_key
      << ";parse-artifact-diagnostics-hardening-key="
      << surface.parse_artifact_diagnostics_hardening_key
      << ";edge-case-robustness-key=" << surface.edge_case_robustness_key;
  return key.str();
}

std::string BuildObjc3IREmissionCoreFeatureRecoveryDeterminismHardeningKey(
    const Objc3IREmissionCoreFeatureImplementationSurface &surface) {
  std::ostringstream key;
  key << "ir-emission-core-feature-recovery-determinism-hardening:v1:"
      << "diagnostics-hardening-ready="
      << (surface.core_feature_diagnostics_hardening_ready ? "true" : "false")
      << ";pass-graph-recovery-determinism-ready="
      << (surface.pass_graph_recovery_determinism_ready ? "true" : "false")
      << ";recovery-determinism-consistent="
      << (surface.recovery_determinism_consistent ? "true" : "false")
      << ";parse-artifact-recovery-determinism-hardening-consistent="
      << (surface.parse_artifact_recovery_determinism_hardening_consistent
              ? "true"
              : "false")
      << ";recovery-determinism-key-transport-ready="
      << (surface.recovery_determinism_key_transport_ready ? "true" : "false")
      << ";recovery-determinism-ready="
      << (surface.core_feature_recovery_determinism_ready ? "true" : "false")
      << ";pass-graph-recovery-determinism-key="
      << surface.pass_graph_recovery_determinism_key
      << ";parse-artifact-recovery-determinism-hardening-key="
      << surface.parse_artifact_recovery_determinism_hardening_key
      << ";diagnostics-hardening-key=" << surface.diagnostics_hardening_key;
  return key.str();
}

std::string BuildObjc3IREmissionCoreFeatureConformanceMatrixKey(
    const Objc3IREmissionCoreFeatureImplementationSurface &surface) {
  std::ostringstream key;
  key << "ir-emission-core-feature-conformance-matrix:v1:"
      << "recovery-determinism-ready="
      << (surface.core_feature_recovery_determinism_ready ? "true" : "false")
      << ";pass-graph-conformance-matrix-ready="
      << (surface.pass_graph_conformance_matrix_ready ? "true" : "false")
      << ";parse-artifact-conformance-matrix-consistent="
      << (surface.parse_artifact_conformance_matrix_consistent ? "true"
                                                               : "false")
      << ";parse-artifact-replay-key-deterministic="
      << (surface.parse_artifact_replay_key_deterministic ? "true" : "false")
      << ";conformance-matrix-consistent="
      << (surface.conformance_matrix_consistent ? "true" : "false")
      << ";conformance-matrix-key-transport-ready="
      << (surface.conformance_matrix_key_transport_ready ? "true" : "false")
      << ";conformance-matrix-ready="
      << (surface.core_feature_conformance_matrix_ready ? "true" : "false")
      << ";pass-graph-conformance-matrix-key="
      << surface.pass_graph_conformance_matrix_key
      << ";parse-artifact-conformance-matrix-key="
      << surface.parse_artifact_conformance_matrix_key
      << ";recovery-determinism-key=" << surface.recovery_determinism_key;
  return key.str();
}

std::string BuildObjc3IREmissionCoreFeatureConformanceCorpusKey(
    const Objc3IREmissionCoreFeatureImplementationSurface &surface) {
  std::ostringstream key;
  key << "ir-emission-core-feature-conformance-corpus:v1:"
      << "conformance-matrix-ready="
      << (surface.core_feature_conformance_matrix_ready ? "true" : "false")
      << ";pass-graph-conformance-corpus-ready="
      << (surface.pass_graph_conformance_corpus_ready ? "true" : "false")
      << ";parse-artifact-conformance-corpus-consistent="
      << (surface.parse_artifact_conformance_corpus_consistent ? "true"
                                                               : "false")
      << ";parse-artifact-replay-key-deterministic="
      << (surface.parse_artifact_replay_key_deterministic ? "true" : "false")
      << ";conformance-corpus-consistent="
      << (surface.conformance_corpus_consistent ? "true" : "false")
      << ";conformance-corpus-key-transport-ready="
      << (surface.conformance_corpus_key_transport_ready ? "true" : "false")
      << ";conformance-corpus-ready="
      << (surface.core_feature_conformance_corpus_ready ? "true" : "false")
      << ";pass-graph-conformance-corpus-key="
      << surface.pass_graph_conformance_corpus_key
      << ";parse-artifact-conformance-corpus-key="
      << surface.parse_artifact_conformance_corpus_key
      << ";conformance-matrix-key=" << surface.conformance_matrix_key;
  return key.str();
}

std::string BuildObjc3IREmissionCoreFeaturePerformanceQualityGuardrailsKey(
    const Objc3IREmissionCoreFeatureImplementationSurface &surface) {
  std::ostringstream key;
  key << "ir-emission-core-feature-performance-quality-guardrails:v1:"
      << "conformance-corpus-ready="
      << (surface.core_feature_conformance_corpus_ready ? "true" : "false")
      << ";pass-graph-performance-quality-guardrails-ready="
      << (surface.pass_graph_performance_quality_guardrails_ready ? "true"
                                                                  : "false")
      << ";parse-artifact-performance-quality-guardrails-consistent="
      << (surface.parse_artifact_performance_quality_guardrails_consistent
              ? "true"
              : "false")
      << ";performance-quality-guardrails-consistent="
      << (surface.performance_quality_guardrails_consistent ? "true" : "false")
      << ";performance-quality-guardrails-key-transport-ready="
      << (surface.performance_quality_guardrails_key_transport_ready ? "true"
                                                                     : "false")
      << ";performance-quality-guardrails-ready="
      << (surface.core_feature_performance_quality_guardrails_ready ? "true"
                                                                    : "false")
      << ";pass-graph-performance-quality-guardrails-key="
      << surface.pass_graph_performance_quality_guardrails_key
      << ";parse-artifact-performance-quality-guardrails-key="
      << surface.parse_artifact_performance_quality_guardrails_key
      << ";conformance-corpus-key=" << surface.conformance_corpus_key;
  return key.str();
}

std::string BuildObjc3IREmissionCoreFeatureCrossLaneIntegrationSyncKey(
    const Objc3IREmissionCoreFeatureImplementationSurface &surface) {
  std::ostringstream key;
  key << "ir-emission-core-feature-cross-lane-integration-sync:v1:"
      << "performance-quality-guardrails-ready="
      << (surface.core_feature_performance_quality_guardrails_ready ? "true"
                                                                    : "false")
      << ";pass-graph-cross-lane-integration-sync-ready="
      << (surface.pass_graph_cross_lane_integration_sync_ready ? "true"
                                                               : "false")
      << ";parse-artifact-cross-lane-integration-sync-consistent="
      << (surface.parse_artifact_cross_lane_integration_sync_consistent
              ? "true"
              : "false")
      << ";cross-lane-integration-sync-consistent="
      << (surface.cross_lane_integration_sync_consistent ? "true" : "false")
      << ";cross-lane-integration-sync-key-transport-ready="
      << (surface.cross_lane_integration_sync_key_transport_ready ? "true"
                                                                  : "false")
      << ";cross-lane-integration-sync-ready="
      << (surface.core_feature_cross_lane_integration_sync_ready ? "true"
                                                                 : "false")
      << ";pass-graph-cross-lane-integration-sync-key="
      << surface.pass_graph_cross_lane_integration_sync_key
      << ";parse-artifact-cross-lane-integration-sync-key="
      << surface.parse_artifact_cross_lane_integration_sync_key
      << ";performance-quality-guardrails-key="
      << surface.performance_quality_guardrails_key;
  return key.str();
}
