#include "pipeline/objc3_ir_emission_core_feature_implementation_surface.h"

namespace objc3_ir_emission_core_feature_surface {

void PublishObjc3IREmissionCoreFeatureSurfaceReadiness(
    Objc3IREmissionCoreFeatureImplementationSurface &surface,
    const Objc3FrontendPipelineResult &pipeline_result) {
  const Objc3IREmissionCompletenessScaffold &scaffold =
      pipeline_result.ir_emission_completeness_scaffold;
  const Objc3ParseLoweringReadinessSurface &parse_surface =
      pipeline_result.parse_lowering_readiness_surface;
  const Objc3TypedSemaToLoweringContractSurface &typed_surface =
      pipeline_result.typed_sema_to_lowering_contract_surface;

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
  surface.pass_graph_advanced_core_shard1_ready =
      surface.core_feature_cross_lane_integration_sync_ready &&
      surface.pass_graph_cross_lane_integration_sync_ready;
  surface.pass_graph_advanced_core_shard1_key =
      surface.cross_lane_integration_sync_key;
  surface.parse_artifact_advanced_core_shard1_consistent =
      parse_surface.toolchain_runtime_ga_operations_advanced_core_consistent &&
      parse_surface.toolchain_runtime_ga_operations_advanced_core_ready;
  const bool typed_advanced_core_shard1_alignment =
      parse_surface.typed_sema_advanced_core_shard1_consistent ==
          typed_surface.typed_advanced_core_shard1_consistent &&
      parse_surface.typed_sema_advanced_core_shard1_ready ==
          typed_surface.typed_advanced_core_shard1_ready &&
      parse_surface.typed_sema_advanced_core_shard1_key ==
          typed_surface.typed_advanced_core_shard1_key;
  surface.typed_handoff_advanced_core_shard1_consistent =
      typed_advanced_core_shard1_alignment &&
      parse_surface.typed_sema_advanced_core_shard1_consistent &&
      parse_surface.typed_sema_advanced_core_shard1_ready &&
      typed_surface.typed_advanced_core_shard1_consistent &&
      typed_surface.typed_advanced_core_shard1_ready;
  surface.advanced_core_shard1_consistent =
      surface.core_feature_cross_lane_integration_sync_ready &&
      surface.pass_graph_advanced_core_shard1_ready &&
      surface.parse_artifact_advanced_core_shard1_consistent &&
      surface.typed_handoff_advanced_core_shard1_consistent;
  surface.advanced_core_shard1_key_transport_ready =
      !surface.pass_graph_advanced_core_shard1_key.empty() &&
      !surface.parse_artifact_advanced_core_shard1_key.empty() &&
      !surface.typed_handoff_advanced_core_shard1_key.empty();
  surface.core_feature_advanced_core_shard1_ready =
      surface.core_feature_cross_lane_integration_sync_ready &&
      surface.pass_graph_advanced_core_shard1_ready &&
      surface.advanced_core_shard1_consistent &&
      surface.advanced_core_shard1_key_transport_ready;
  surface.advanced_core_shard1_key =
      BuildObjc3IREmissionCoreFeatureAdvancedCoreShard1Key(surface);
  surface.pass_graph_advanced_edge_compatibility_shard1_ready =
      surface.core_feature_advanced_core_shard1_ready &&
      surface.pass_graph_advanced_core_shard1_ready;
  surface.pass_graph_advanced_edge_compatibility_shard1_key =
      surface.advanced_core_shard1_key;
  surface.parse_artifact_advanced_edge_compatibility_shard1_consistent =
      parse_surface
          .toolchain_runtime_ga_operations_advanced_edge_compatibility_consistent &&
      parse_surface.toolchain_runtime_ga_operations_advanced_edge_compatibility_ready;
  const bool typed_advanced_edge_compatibility_shard1_alignment =
      parse_surface.typed_sema_advanced_edge_compatibility_shard1_consistent ==
          typed_surface.typed_advanced_edge_compatibility_shard1_consistent &&
      parse_surface.typed_sema_advanced_edge_compatibility_shard1_ready ==
          typed_surface.typed_advanced_edge_compatibility_shard1_ready &&
      parse_surface.typed_sema_advanced_edge_compatibility_shard1_key ==
          typed_surface.typed_advanced_edge_compatibility_shard1_key;
  surface.typed_handoff_advanced_edge_compatibility_shard1_consistent =
      typed_advanced_edge_compatibility_shard1_alignment &&
      parse_surface.typed_sema_advanced_edge_compatibility_shard1_consistent &&
      parse_surface.typed_sema_advanced_edge_compatibility_shard1_ready &&
      typed_surface.typed_advanced_edge_compatibility_shard1_consistent &&
      typed_surface.typed_advanced_edge_compatibility_shard1_ready;
  surface.advanced_edge_compatibility_shard1_consistent =
      surface.core_feature_advanced_core_shard1_ready &&
      surface.pass_graph_advanced_edge_compatibility_shard1_ready &&
      surface.parse_artifact_advanced_edge_compatibility_shard1_consistent &&
      surface.typed_handoff_advanced_edge_compatibility_shard1_consistent;
  surface.advanced_edge_compatibility_shard1_key_transport_ready =
      !surface.pass_graph_advanced_edge_compatibility_shard1_key.empty() &&
      !surface.parse_artifact_advanced_edge_compatibility_shard1_key.empty() &&
      !surface.typed_handoff_advanced_edge_compatibility_shard1_key.empty();
  surface.core_feature_advanced_edge_compatibility_shard1_ready =
      surface.core_feature_advanced_core_shard1_ready &&
      surface.pass_graph_advanced_edge_compatibility_shard1_ready &&
      surface.advanced_edge_compatibility_shard1_consistent &&
      surface.advanced_edge_compatibility_shard1_key_transport_ready;
  surface.advanced_edge_compatibility_shard1_key =
      BuildObjc3IREmissionCoreFeatureAdvancedEdgeCompatibilityShard1Key(
          surface);
  surface.pass_graph_advanced_diagnostics_shard1_ready =
      surface.core_feature_advanced_edge_compatibility_shard1_ready &&
      surface.pass_graph_advanced_edge_compatibility_shard1_ready;
  surface.pass_graph_advanced_diagnostics_shard1_key =
      surface.advanced_edge_compatibility_shard1_key;
  surface.parse_artifact_advanced_diagnostics_shard1_consistent =
      parse_surface.toolchain_runtime_ga_operations_advanced_diagnostics_consistent &&
      parse_surface.toolchain_runtime_ga_operations_advanced_diagnostics_ready;
  const bool typed_advanced_diagnostics_shard1_alignment =
      parse_surface.typed_sema_advanced_diagnostics_shard1_consistent ==
          typed_surface.typed_advanced_diagnostics_shard1_consistent &&
      parse_surface.typed_sema_advanced_diagnostics_shard1_ready ==
          typed_surface.typed_advanced_diagnostics_shard1_ready &&
      parse_surface.typed_sema_advanced_diagnostics_shard1_key ==
          typed_surface.typed_advanced_diagnostics_shard1_key;
  surface.typed_handoff_advanced_diagnostics_shard1_consistent =
      typed_advanced_diagnostics_shard1_alignment &&
      parse_surface.typed_sema_advanced_diagnostics_shard1_consistent &&
      parse_surface.typed_sema_advanced_diagnostics_shard1_ready &&
      typed_surface.typed_advanced_diagnostics_shard1_consistent &&
      typed_surface.typed_advanced_diagnostics_shard1_ready;
  surface.advanced_diagnostics_shard1_consistent =
      surface.core_feature_advanced_edge_compatibility_shard1_ready &&
      surface.pass_graph_advanced_diagnostics_shard1_ready &&
      surface.parse_artifact_advanced_diagnostics_shard1_consistent &&
      surface.typed_handoff_advanced_diagnostics_shard1_consistent;
  surface.advanced_diagnostics_shard1_key_transport_ready =
      !surface.pass_graph_advanced_diagnostics_shard1_key.empty() &&
      !surface.parse_artifact_advanced_diagnostics_shard1_key.empty() &&
      !surface.typed_handoff_advanced_diagnostics_shard1_key.empty();
  surface.core_feature_advanced_diagnostics_shard1_ready =
      surface.core_feature_advanced_edge_compatibility_shard1_ready &&
      surface.pass_graph_advanced_diagnostics_shard1_ready &&
      surface.advanced_diagnostics_shard1_consistent &&
      surface.advanced_diagnostics_shard1_key_transport_ready;
  surface.advanced_diagnostics_shard1_key =
      BuildObjc3IREmissionCoreFeatureAdvancedDiagnosticsShard1Key(surface);
  surface.pass_graph_advanced_conformance_shard1_ready =
      surface.core_feature_advanced_diagnostics_shard1_ready &&
      surface.pass_graph_advanced_diagnostics_shard1_ready;
  surface.pass_graph_advanced_conformance_shard1_key =
      surface.advanced_diagnostics_shard1_key;
  surface.parse_artifact_advanced_conformance_shard1_consistent =
      parse_surface.toolchain_runtime_ga_operations_advanced_conformance_consistent &&
      parse_surface.toolchain_runtime_ga_operations_advanced_conformance_ready;
  const bool typed_advanced_conformance_shard1_alignment =
      parse_surface.typed_sema_advanced_conformance_shard1_consistent ==
          typed_surface.typed_advanced_conformance_shard1_consistent &&
      parse_surface.typed_sema_advanced_conformance_shard1_ready ==
          typed_surface.typed_advanced_conformance_shard1_ready &&
      parse_surface.typed_sema_advanced_conformance_shard1_key ==
          typed_surface.typed_advanced_conformance_shard1_key;
  surface.typed_handoff_advanced_conformance_shard1_consistent =
      typed_advanced_conformance_shard1_alignment &&
      parse_surface.typed_sema_advanced_conformance_shard1_consistent &&
      parse_surface.typed_sema_advanced_conformance_shard1_ready &&
      typed_surface.typed_advanced_conformance_shard1_consistent &&
      typed_surface.typed_advanced_conformance_shard1_ready;
  surface.advanced_conformance_shard1_consistent =
      surface.core_feature_advanced_diagnostics_shard1_ready &&
      surface.pass_graph_advanced_conformance_shard1_ready &&
      surface.parse_artifact_advanced_conformance_shard1_consistent &&
      surface.typed_handoff_advanced_conformance_shard1_consistent;
  surface.advanced_conformance_shard1_key_transport_ready =
      !surface.pass_graph_advanced_conformance_shard1_key.empty() &&
      !surface.parse_artifact_advanced_conformance_shard1_key.empty() &&
      !surface.typed_handoff_advanced_conformance_shard1_key.empty();
  surface.core_feature_advanced_conformance_shard1_ready =
      surface.core_feature_advanced_diagnostics_shard1_ready &&
      surface.pass_graph_advanced_conformance_shard1_ready &&
      surface.advanced_conformance_shard1_consistent &&
      surface.advanced_conformance_shard1_key_transport_ready;
  surface.advanced_conformance_shard1_key =
      BuildObjc3IREmissionCoreFeatureAdvancedConformanceShard1Key(surface);
  surface.pass_graph_advanced_integration_shard1_ready =
      surface.core_feature_advanced_conformance_shard1_ready &&
      surface.pass_graph_advanced_conformance_shard1_ready;
  surface.pass_graph_advanced_integration_shard1_key =
      surface.advanced_conformance_shard1_key;
  surface.parse_artifact_advanced_integration_shard1_consistent =
      parse_surface.toolchain_runtime_ga_operations_advanced_integration_consistent &&
      parse_surface.toolchain_runtime_ga_operations_advanced_integration_ready;
  const bool typed_advanced_integration_shard1_alignment =
      parse_surface.typed_sema_advanced_integration_shard1_consistent ==
          typed_surface.typed_advanced_integration_shard1_consistent &&
      parse_surface.typed_sema_advanced_integration_shard1_ready ==
          typed_surface.typed_advanced_integration_shard1_ready &&
      parse_surface.typed_sema_advanced_integration_shard1_key ==
          typed_surface.typed_advanced_integration_shard1_key;
  surface.typed_handoff_advanced_integration_shard1_consistent =
      typed_advanced_integration_shard1_alignment &&
      parse_surface.typed_sema_advanced_integration_shard1_consistent &&
      parse_surface.typed_sema_advanced_integration_shard1_ready &&
      typed_surface.typed_advanced_integration_shard1_consistent &&
      typed_surface.typed_advanced_integration_shard1_ready;
  surface.advanced_integration_shard1_consistent =
      surface.core_feature_advanced_conformance_shard1_ready &&
      surface.pass_graph_advanced_integration_shard1_ready &&
      surface.parse_artifact_advanced_integration_shard1_consistent &&
      surface.typed_handoff_advanced_integration_shard1_consistent;
  surface.advanced_integration_shard1_key_transport_ready =
      !surface.pass_graph_advanced_integration_shard1_key.empty() &&
      !surface.parse_artifact_advanced_integration_shard1_key.empty() &&
      !surface.typed_handoff_advanced_integration_shard1_key.empty();
  surface.core_feature_advanced_integration_shard1_ready =
      surface.core_feature_advanced_conformance_shard1_ready &&
      surface.pass_graph_advanced_integration_shard1_ready &&
      surface.advanced_integration_shard1_consistent &&
      surface.advanced_integration_shard1_key_transport_ready;
  surface.advanced_integration_shard1_key =
      BuildObjc3IREmissionCoreFeatureAdvancedIntegrationShard1Key(surface);
}

}  // namespace objc3_ir_emission_core_feature_surface
