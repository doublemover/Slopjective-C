#pragma once

#include <sstream>
#include <string>

#include "pipeline/objc3_frontend_types.h"

struct Objc3IREmissionCoreFeatureImplementationSurface {
  bool modular_split_ready = false;
  bool metadata_transport_ready = false;
  bool pass_graph_core_feature_ready = false;
  bool pass_graph_expansion_ready = false;
  bool pass_graph_edge_case_compatibility_ready = false;
  bool pass_graph_edge_case_robustness_ready = false;
  bool pass_graph_diagnostics_hardening_ready = false;
  bool pass_graph_recovery_determinism_ready = false;
  bool pass_graph_conformance_matrix_ready = false;
  bool pass_graph_conformance_corpus_ready = false;
  bool pass_graph_performance_quality_guardrails_ready = false;
  bool pass_graph_cross_lane_integration_sync_ready = false;
  bool runtime_boundary_handoff_ready = false;
  bool direct_ir_entrypoint_ready = false;
  bool expansion_metadata_transport_ready = false;
  bool compatibility_handoff_consistent = false;
  bool language_version_pragma_coordinate_order_consistent = false;
  bool parse_artifact_edge_case_robustness_consistent = false;
  bool parse_artifact_diagnostics_hardening_consistent = false;
  bool parse_artifact_recovery_determinism_hardening_consistent = false;
  bool parse_artifact_conformance_matrix_consistent = false;
  bool parse_artifact_conformance_corpus_consistent = false;
  bool parse_artifact_performance_quality_guardrails_consistent = false;
  bool parse_artifact_cross_lane_integration_sync_consistent = false;
  bool edge_case_expansion_consistent = false;
  bool diagnostics_hardening_consistent = false;
  bool recovery_determinism_consistent = false;
  bool conformance_matrix_consistent = false;
  bool conformance_corpus_consistent = false;
  bool performance_quality_guardrails_consistent = false;
  bool cross_lane_integration_sync_consistent = false;
  bool parse_artifact_edge_case_robustness_ready = false;
  bool parse_artifact_replay_key_deterministic = false;
  bool edge_case_compatibility_key_transport_ready = false;
  bool edge_case_robustness_key_transport_ready = false;
  bool diagnostics_hardening_key_transport_ready = false;
  bool recovery_determinism_key_transport_ready = false;
  bool conformance_matrix_key_transport_ready = false;
  bool conformance_corpus_key_transport_ready = false;
  bool performance_quality_guardrails_key_transport_ready = false;
  bool cross_lane_integration_sync_key_transport_ready = false;
  bool core_feature_impl_ready = false;
  bool core_feature_expansion_ready = false;
  bool core_feature_edge_case_compatibility_ready = false;
  bool core_feature_edge_case_robustness_ready = false;
  bool core_feature_diagnostics_hardening_ready = false;
  bool core_feature_recovery_determinism_ready = false;
  bool core_feature_conformance_matrix_ready = false;
  bool core_feature_conformance_corpus_ready = false;
  bool core_feature_performance_quality_guardrails_ready = false;
  bool core_feature_cross_lane_integration_sync_ready = false;
  std::string scaffold_key;
  std::string core_feature_key;
  std::string expansion_key;
  std::string pass_graph_edge_case_compatibility_key;
  std::string pass_graph_edge_case_robustness_key;
  std::string pass_graph_diagnostics_hardening_key;
  std::string pass_graph_recovery_determinism_key;
  std::string pass_graph_conformance_matrix_key;
  std::string pass_graph_conformance_corpus_key;
  std::string pass_graph_performance_quality_guardrails_key;
  std::string pass_graph_cross_lane_integration_sync_key;
  std::string compatibility_handoff_key;
  std::string parse_artifact_diagnostics_hardening_key;
  std::string parse_artifact_recovery_determinism_hardening_key;
  std::string parse_artifact_conformance_matrix_key;
  std::string parse_artifact_conformance_corpus_key;
  std::string parse_artifact_performance_quality_guardrails_key;
  std::string parse_artifact_cross_lane_integration_sync_key;
  std::string parse_artifact_edge_case_expansion_key;
  std::string parse_artifact_edge_robustness_key;
  std::string edge_case_compatibility_key;
  std::string edge_case_robustness_key;
  std::string diagnostics_hardening_key;
  std::string recovery_determinism_key;
  std::string conformance_matrix_key;
  std::string conformance_corpus_key;
  std::string performance_quality_guardrails_key;
  std::string cross_lane_integration_sync_key;
  std::string failure_reason;
  std::string expansion_failure_reason;
  std::string edge_case_compatibility_failure_reason;
  std::string edge_case_robustness_failure_reason;
  std::string diagnostics_hardening_failure_reason;
  std::string recovery_determinism_failure_reason;
  std::string conformance_matrix_failure_reason;
  std::string conformance_corpus_failure_reason;
  std::string performance_quality_guardrails_failure_reason;
  std::string cross_lane_integration_sync_failure_reason;
};

inline std::string BuildObjc3IREmissionCoreFeatureImplementationKey(
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

inline std::string BuildObjc3IREmissionCoreFeatureExpansionKey(
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

inline std::string BuildObjc3IREmissionCoreFeatureEdgeCaseCompatibilityKey(
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

inline std::string BuildObjc3IREmissionCoreFeatureEdgeCaseRobustnessKey(
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

inline std::string BuildObjc3IREmissionCoreFeatureDiagnosticsHardeningKey(
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

inline std::string BuildObjc3IREmissionCoreFeatureRecoveryDeterminismHardeningKey(
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

inline std::string BuildObjc3IREmissionCoreFeatureConformanceMatrixKey(
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

inline std::string BuildObjc3IREmissionCoreFeatureConformanceCorpusKey(
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

inline std::string BuildObjc3IREmissionCoreFeaturePerformanceQualityGuardrailsKey(
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

inline std::string BuildObjc3IREmissionCoreFeatureCrossLaneIntegrationSyncKey(
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

inline Objc3IREmissionCoreFeatureImplementationSurface
BuildObjc3IREmissionCoreFeatureImplementationSurface(
    const Objc3FrontendPipelineResult &pipeline_result) {
  Objc3IREmissionCoreFeatureImplementationSurface surface;
  const Objc3IREmissionCompletenessScaffold &scaffold =
      pipeline_result.ir_emission_completeness_scaffold;
  const Objc3ParseLoweringReadinessSurface &parse_surface =
      pipeline_result.parse_lowering_readiness_surface;
  const Objc3TypedSemaToLoweringContractSurface &typed_surface =
      pipeline_result.typed_sema_to_lowering_contract_surface;

  surface.modular_split_ready = scaffold.modular_split_ready;
  surface.metadata_transport_ready = scaffold.metadata_transport_ready;
  surface.pass_graph_core_feature_ready = scaffold.core_feature_ready;
  surface.pass_graph_expansion_ready = scaffold.expansion_ready;
  surface.pass_graph_edge_case_compatibility_ready =
      scaffold.edge_case_compatibility_ready;
  surface.pass_graph_edge_case_robustness_ready =
      pipeline_result.lowering_pipeline_pass_graph_core_feature_surface
          .edge_case_robustness_ready;
  surface.pass_graph_diagnostics_hardening_ready =
      pipeline_result.lowering_pipeline_pass_graph_core_feature_surface
          .diagnostics_hardening_ready;
  surface.pass_graph_recovery_determinism_ready =
      pipeline_result.lowering_pipeline_pass_graph_core_feature_surface
          .recovery_determinism_ready;
  surface.pass_graph_conformance_matrix_ready =
      pipeline_result.lowering_pipeline_pass_graph_core_feature_surface
          .conformance_matrix_ready;
  surface.pass_graph_conformance_corpus_ready =
      pipeline_result.lowering_pipeline_pass_graph_core_feature_surface
          .conformance_corpus_ready;
  surface.pass_graph_performance_quality_guardrails_ready =
      pipeline_result.lowering_pipeline_pass_graph_core_feature_surface
          .performance_quality_guardrails_ready;
  surface.pass_graph_cross_lane_integration_sync_ready =
      pipeline_result.lowering_pipeline_pass_graph_core_feature_surface
          .performance_quality_guardrails_ready;
  surface.runtime_boundary_handoff_ready =
      typed_surface.lowering_boundary_ready &&
      !typed_surface.lowering_boundary_replay_key.empty();
  surface.direct_ir_entrypoint_ready =
      pipeline_result.lowering_pipeline_pass_graph_scaffold
          .ir_emission_entrypoint_ready;
  surface.compatibility_handoff_consistent =
      parse_surface.compatibility_handoff_consistent;
  surface.language_version_pragma_coordinate_order_consistent =
      parse_surface.language_version_pragma_coordinate_order_consistent;
  surface.parse_artifact_edge_case_robustness_consistent =
      parse_surface.parse_artifact_edge_case_robustness_consistent;
  surface.parse_artifact_diagnostics_hardening_consistent =
      parse_surface.parse_artifact_diagnostics_hardening_consistent;
  surface.parse_artifact_recovery_determinism_hardening_consistent =
      parse_surface.parse_recovery_determinism_hardening_consistent;
  surface.parse_artifact_conformance_matrix_consistent =
      parse_surface.parse_lowering_conformance_matrix_consistent;
  surface.parse_artifact_conformance_corpus_consistent =
      parse_surface.parse_lowering_conformance_corpus_consistent;
  surface.parse_artifact_performance_quality_guardrails_consistent =
      parse_surface.parse_lowering_performance_quality_guardrails_consistent;
  surface.parse_artifact_cross_lane_integration_sync_consistent =
      parse_surface.typed_sema_cross_lane_integration_consistent &&
      parse_surface.toolchain_runtime_ga_operations_cross_lane_integration_consistent;
  surface.edge_case_expansion_consistent =
      parse_surface.long_tail_grammar_edge_case_expansion_consistent;
  surface.parse_artifact_edge_case_robustness_ready =
      parse_surface.long_tail_grammar_edge_case_robustness_ready;
  surface.parse_artifact_replay_key_deterministic =
      parse_surface.parse_artifact_replay_key_deterministic;
  surface.scaffold_key = scaffold.scaffold_key;
  surface.pass_graph_edge_case_compatibility_key =
      scaffold.edge_case_compatibility_key;
  surface.pass_graph_edge_case_robustness_key =
      pipeline_result.lowering_pipeline_pass_graph_core_feature_surface
          .edge_case_robustness_key;
  surface.pass_graph_diagnostics_hardening_key =
      pipeline_result.lowering_pipeline_pass_graph_core_feature_surface
          .diagnostics_hardening_key;
  surface.pass_graph_recovery_determinism_key =
      pipeline_result.lowering_pipeline_pass_graph_core_feature_surface
          .recovery_determinism_key;
  surface.pass_graph_conformance_matrix_key =
      pipeline_result.lowering_pipeline_pass_graph_core_feature_surface
          .conformance_matrix_key;
  surface.pass_graph_conformance_corpus_key =
      pipeline_result.lowering_pipeline_pass_graph_core_feature_surface
          .conformance_corpus_key;
  surface.pass_graph_performance_quality_guardrails_key =
      pipeline_result.lowering_pipeline_pass_graph_core_feature_surface
          .performance_quality_guardrails_key;
  surface.pass_graph_cross_lane_integration_sync_key =
      pipeline_result.lowering_pipeline_pass_graph_core_feature_surface
          .performance_quality_guardrails_key;
  surface.compatibility_handoff_key = parse_surface.compatibility_handoff_key;
  surface.parse_artifact_diagnostics_hardening_key =
      parse_surface.parse_artifact_diagnostics_hardening_key;
  surface.parse_artifact_recovery_determinism_hardening_key =
      parse_surface.parse_recovery_determinism_hardening_key;
  surface.parse_artifact_conformance_matrix_key =
      parse_surface.parse_lowering_conformance_matrix_key;
  surface.parse_artifact_conformance_corpus_key =
      parse_surface.parse_lowering_conformance_corpus_key;
  surface.parse_artifact_performance_quality_guardrails_key =
      parse_surface.parse_lowering_performance_quality_guardrails_key;
  surface.parse_artifact_cross_lane_integration_sync_key =
      parse_surface.typed_sema_cross_lane_integration_key + "|" +
      parse_surface.toolchain_runtime_ga_operations_cross_lane_integration_key +
      "|" + parse_surface.parse_lowering_performance_quality_guardrails_key;
  surface.parse_artifact_edge_case_expansion_key =
      parse_surface.long_tail_grammar_expansion_key;
  surface.parse_artifact_edge_robustness_key =
      parse_surface.parse_artifact_edge_robustness_key;

  surface.core_feature_impl_ready =
      surface.modular_split_ready && surface.metadata_transport_ready &&
      surface.pass_graph_core_feature_ready &&
      surface.runtime_boundary_handoff_ready &&
      surface.direct_ir_entrypoint_ready && !surface.scaffold_key.empty();
  surface.core_feature_key =
      BuildObjc3IREmissionCoreFeatureImplementationKey(surface);
  surface.expansion_metadata_transport_ready = !scaffold.expansion_key.empty();
  surface.core_feature_expansion_ready =
      surface.core_feature_impl_ready && surface.pass_graph_expansion_ready &&
      surface.expansion_metadata_transport_ready &&
      surface.runtime_boundary_handoff_ready &&
      surface.direct_ir_entrypoint_ready;
  surface.expansion_key = BuildObjc3IREmissionCoreFeatureExpansionKey(surface);
  surface.edge_case_compatibility_key_transport_ready =
      !surface.pass_graph_edge_case_compatibility_key.empty() &&
      !surface.compatibility_handoff_key.empty() &&
      !surface.parse_artifact_edge_robustness_key.empty();
  surface.core_feature_edge_case_compatibility_ready =
      surface.core_feature_expansion_ready &&
      surface.pass_graph_edge_case_compatibility_ready &&
      surface.compatibility_handoff_consistent &&
      surface.language_version_pragma_coordinate_order_consistent &&
      surface.parse_artifact_edge_case_robustness_consistent &&
      surface.parse_artifact_replay_key_deterministic &&
      surface.edge_case_compatibility_key_transport_ready;
  surface.edge_case_compatibility_key =
      BuildObjc3IREmissionCoreFeatureEdgeCaseCompatibilityKey(surface);
  surface.edge_case_robustness_key_transport_ready =
      !surface.pass_graph_edge_case_robustness_key.empty() &&
      !surface.parse_artifact_edge_case_expansion_key.empty() &&
      !surface.parse_artifact_edge_robustness_key.empty() &&
      !surface.edge_case_compatibility_key.empty();
  surface.core_feature_edge_case_robustness_ready =
      surface.core_feature_edge_case_compatibility_ready &&
      surface.pass_graph_edge_case_robustness_ready &&
      surface.edge_case_expansion_consistent &&
      surface.parse_artifact_edge_case_robustness_ready &&
      surface.edge_case_robustness_key_transport_ready;
  surface.edge_case_robustness_key =
      BuildObjc3IREmissionCoreFeatureEdgeCaseRobustnessKey(surface);
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

  if (surface.core_feature_expansion_ready) {
    surface.expansion_failure_reason.clear();
  } else if (!surface.core_feature_impl_ready) {
    surface.expansion_failure_reason =
        "IR emission core feature implementation is not ready";
  } else if (!surface.pass_graph_expansion_ready) {
    surface.expansion_failure_reason = "pass-graph expansion is not ready";
  } else if (!surface.expansion_metadata_transport_ready) {
    surface.expansion_failure_reason =
        "IR emission core feature expansion metadata transport is not ready";
  } else if (!surface.runtime_boundary_handoff_ready) {
    surface.expansion_failure_reason =
        "runtime boundary handoff replay surface is not ready";
  } else if (!surface.direct_ir_entrypoint_ready) {
    surface.expansion_failure_reason = "direct IR entrypoint is not ready";
  } else {
    surface.expansion_failure_reason =
        "IR emission core feature expansion surface is not ready";
  }

  if (surface.core_feature_edge_case_compatibility_ready) {
    surface.edge_case_compatibility_failure_reason.clear();
  } else if (!surface.core_feature_expansion_ready) {
    surface.edge_case_compatibility_failure_reason =
        "IR emission core feature expansion is not ready";
  } else if (!surface.pass_graph_edge_case_compatibility_ready) {
    surface.edge_case_compatibility_failure_reason =
        "pass-graph edge-case compatibility is not ready";
  } else if (!surface.compatibility_handoff_consistent) {
    surface.edge_case_compatibility_failure_reason =
        "IR emission core feature compatibility handoff is inconsistent";
  } else if (!surface.language_version_pragma_coordinate_order_consistent) {
    surface.edge_case_compatibility_failure_reason =
        "IR emission core feature language version pragma coordinate order is inconsistent";
  } else if (!surface.parse_artifact_edge_case_robustness_consistent) {
    surface.edge_case_compatibility_failure_reason =
        "IR emission core feature parse artifact edge-case robustness is inconsistent";
  } else if (!surface.parse_artifact_replay_key_deterministic) {
    surface.edge_case_compatibility_failure_reason =
        "IR emission core feature parse artifact replay key is not deterministic";
  } else if (!surface.edge_case_compatibility_key_transport_ready) {
    surface.edge_case_compatibility_failure_reason =
        "IR emission core feature edge-case compatibility key transport is not ready";
  } else {
    surface.edge_case_compatibility_failure_reason =
        "IR emission core feature edge-case compatibility surface is not ready";
  }

  if (surface.core_feature_edge_case_robustness_ready) {
    surface.edge_case_robustness_failure_reason.clear();
  } else if (!surface.core_feature_edge_case_compatibility_ready) {
    surface.edge_case_robustness_failure_reason =
        "IR emission core feature edge-case compatibility is not ready";
  } else if (!surface.pass_graph_edge_case_robustness_ready) {
    surface.edge_case_robustness_failure_reason =
        "pass-graph edge-case robustness is not ready";
  } else if (!surface.edge_case_expansion_consistent) {
    surface.edge_case_robustness_failure_reason =
        "IR emission core feature edge-case expansion is inconsistent";
  } else if (!surface.parse_artifact_edge_case_robustness_ready) {
    surface.edge_case_robustness_failure_reason =
        "IR emission core feature parse artifact edge-case robustness is not ready";
  } else if (!surface.edge_case_robustness_key_transport_ready) {
    surface.edge_case_robustness_failure_reason =
        "IR emission core feature edge-case robustness key transport is not ready";
  } else {
    surface.edge_case_robustness_failure_reason =
        "IR emission core feature edge-case robustness surface is not ready";
  }

  if (surface.core_feature_diagnostics_hardening_ready) {
    surface.diagnostics_hardening_failure_reason.clear();
  } else if (!surface.core_feature_edge_case_robustness_ready) {
    surface.diagnostics_hardening_failure_reason =
        "IR emission core feature edge-case robustness is not ready";
  } else if (!surface.pass_graph_diagnostics_hardening_ready) {
    surface.diagnostics_hardening_failure_reason =
        "pass-graph diagnostics hardening is not ready";
  } else if (!surface.parse_artifact_diagnostics_hardening_consistent) {
    surface.diagnostics_hardening_failure_reason =
        "IR emission core feature parse artifact diagnostics hardening is inconsistent";
  } else if (!surface.diagnostics_hardening_consistent) {
    surface.diagnostics_hardening_failure_reason =
        "IR emission core feature diagnostics hardening is inconsistent";
  } else if (!surface.diagnostics_hardening_key_transport_ready) {
    surface.diagnostics_hardening_failure_reason =
        "IR emission core feature diagnostics hardening key transport is not ready";
  } else {
    surface.diagnostics_hardening_failure_reason =
        "IR emission core feature diagnostics hardening surface is not ready";
  }

  if (surface.core_feature_recovery_determinism_ready) {
    surface.recovery_determinism_failure_reason.clear();
  } else if (!surface.core_feature_diagnostics_hardening_ready) {
    surface.recovery_determinism_failure_reason =
        "IR emission core feature diagnostics hardening is not ready";
  } else if (!surface.pass_graph_recovery_determinism_ready) {
    surface.recovery_determinism_failure_reason =
        "pass-graph recovery determinism is not ready";
  } else if (!surface.parse_artifact_recovery_determinism_hardening_consistent) {
    surface.recovery_determinism_failure_reason =
        "IR emission core feature parse artifact recovery determinism hardening is inconsistent";
  } else if (!surface.recovery_determinism_consistent) {
    surface.recovery_determinism_failure_reason =
        "IR emission core feature recovery determinism hardening is inconsistent";
  } else if (!surface.recovery_determinism_key_transport_ready) {
    surface.recovery_determinism_failure_reason =
        "IR emission core feature recovery determinism hardening key transport is not ready";
  } else {
    surface.recovery_determinism_failure_reason =
        "IR emission core feature recovery determinism hardening surface is not ready";
  }

  if (surface.core_feature_conformance_matrix_ready) {
    surface.conformance_matrix_failure_reason.clear();
  } else if (!surface.core_feature_recovery_determinism_ready) {
    surface.conformance_matrix_failure_reason =
        "IR emission core feature recovery determinism hardening is not ready";
  } else if (!surface.pass_graph_conformance_matrix_ready) {
    surface.conformance_matrix_failure_reason =
        "pass-graph conformance matrix is not ready";
  } else if (!surface.parse_artifact_conformance_matrix_consistent) {
    surface.conformance_matrix_failure_reason =
        "IR emission core feature parse artifact conformance matrix is inconsistent";
  } else if (!surface.conformance_matrix_consistent) {
    surface.conformance_matrix_failure_reason =
        "IR emission core feature conformance matrix is inconsistent";
  } else if (!surface.conformance_matrix_key_transport_ready) {
    surface.conformance_matrix_failure_reason =
        "IR emission core feature conformance matrix key transport is not ready";
  } else {
    surface.conformance_matrix_failure_reason =
        "IR emission core feature conformance matrix surface is not ready";
  }

  if (surface.core_feature_conformance_corpus_ready) {
    surface.conformance_corpus_failure_reason.clear();
  } else if (!surface.core_feature_conformance_matrix_ready) {
    surface.conformance_corpus_failure_reason =
        "IR emission core feature conformance matrix is not ready";
  } else if (!surface.pass_graph_conformance_corpus_ready) {
    surface.conformance_corpus_failure_reason =
        "pass-graph conformance corpus is not ready";
  } else if (!surface.parse_artifact_conformance_corpus_consistent) {
    surface.conformance_corpus_failure_reason =
        "IR emission core feature parse artifact conformance corpus is inconsistent";
  } else if (!surface.conformance_corpus_consistent) {
    surface.conformance_corpus_failure_reason =
        "IR emission core feature conformance corpus is inconsistent";
  } else if (!surface.conformance_corpus_key_transport_ready) {
    surface.conformance_corpus_failure_reason =
        "IR emission core feature conformance corpus key transport is not ready";
  } else {
    surface.conformance_corpus_failure_reason =
        "IR emission core feature conformance corpus surface is not ready";
  }

  if (surface.core_feature_performance_quality_guardrails_ready) {
    surface.performance_quality_guardrails_failure_reason.clear();
  } else if (!surface.core_feature_conformance_corpus_ready) {
    surface.performance_quality_guardrails_failure_reason =
        "IR emission core feature conformance corpus is not ready";
  } else if (!surface.pass_graph_performance_quality_guardrails_ready) {
    surface.performance_quality_guardrails_failure_reason =
        "pass-graph performance quality guardrails are not ready";
  } else if (!surface.parse_artifact_performance_quality_guardrails_consistent) {
    surface.performance_quality_guardrails_failure_reason =
        "IR emission core feature parse artifact performance quality guardrails are inconsistent";
  } else if (!surface.performance_quality_guardrails_consistent) {
    surface.performance_quality_guardrails_failure_reason =
        "IR emission core feature performance quality guardrails are inconsistent";
  } else if (!surface.performance_quality_guardrails_key_transport_ready) {
    surface.performance_quality_guardrails_failure_reason =
        "IR emission core feature performance quality guardrails key transport is not ready";
  } else {
    surface.performance_quality_guardrails_failure_reason =
        "IR emission core feature performance quality guardrails surface is not ready";
  }

  if (surface.core_feature_cross_lane_integration_sync_ready) {
    surface.cross_lane_integration_sync_failure_reason.clear();
  } else if (!surface.core_feature_performance_quality_guardrails_ready) {
    surface.cross_lane_integration_sync_failure_reason =
        "IR emission core feature performance quality guardrails are not ready";
  } else if (!surface.pass_graph_cross_lane_integration_sync_ready) {
    surface.cross_lane_integration_sync_failure_reason =
        "pass-graph cross-lane integration sync is not ready";
  } else if (!surface.parse_artifact_cross_lane_integration_sync_consistent) {
    surface.cross_lane_integration_sync_failure_reason =
        "IR emission core feature parse artifact cross-lane integration sync is inconsistent";
  } else if (!surface.cross_lane_integration_sync_consistent) {
    surface.cross_lane_integration_sync_failure_reason =
        "IR emission core feature cross-lane integration sync is inconsistent";
  } else if (!surface.cross_lane_integration_sync_key_transport_ready) {
    surface.cross_lane_integration_sync_failure_reason =
        "IR emission core feature cross-lane integration sync key transport is not ready";
  } else {
    surface.cross_lane_integration_sync_failure_reason =
        "IR emission core feature cross-lane integration sync surface is not ready";
  }

  if (surface.core_feature_impl_ready) {
    return surface;
  }

  if (!surface.modular_split_ready) {
    surface.failure_reason =
        "IR emission completeness modular split scaffold is not ready";
  } else if (!surface.metadata_transport_ready) {
    surface.failure_reason =
        "IR emission completeness metadata transport is not ready";
  } else if (!surface.pass_graph_core_feature_ready) {
    surface.failure_reason = "pass-graph core feature is not ready";
  } else if (!surface.runtime_boundary_handoff_ready) {
    surface.failure_reason =
        "runtime boundary handoff replay surface is not ready";
  } else if (!surface.direct_ir_entrypoint_ready) {
    surface.failure_reason = "direct IR entrypoint is not ready";
  } else {
    surface.failure_reason =
        "IR emission core feature implementation surface is not ready";
  }
  return surface;
}

inline bool IsObjc3IREmissionCoreFeatureImplementationReady(
    const Objc3IREmissionCoreFeatureImplementationSurface &surface,
    std::string &reason) {
  if (surface.core_feature_impl_ready) {
    reason.clear();
    return true;
  }
  reason =
      surface.failure_reason.empty()
          ? "IR emission core feature implementation surface is not ready"
          : surface.failure_reason;
  return false;
}

inline bool IsObjc3IREmissionCoreFeatureExpansionReady(
    const Objc3IREmissionCoreFeatureImplementationSurface &surface,
    std::string &reason) {
  if (surface.core_feature_expansion_ready) {
    reason.clear();
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
