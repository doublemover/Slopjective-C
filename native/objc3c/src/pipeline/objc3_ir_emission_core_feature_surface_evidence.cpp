#include "pipeline/objc3_ir_emission_core_feature_implementation_surface.h"

namespace objc3_ir_emission_core_feature_surface {

void PopulateObjc3IREmissionCoreFeatureSurfaceEvidence(
    Objc3IREmissionCoreFeatureImplementationSurface &surface,
    const Objc3FrontendPipelineResult &pipeline_result) {
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
  surface.parse_artifact_advanced_core_shard1_key =
      parse_surface.toolchain_runtime_ga_operations_advanced_core_key;
  surface.typed_handoff_advanced_core_shard1_key =
      typed_surface.typed_advanced_core_shard1_key;
  surface.parse_artifact_advanced_edge_compatibility_shard1_key =
      parse_surface.toolchain_runtime_ga_operations_advanced_edge_compatibility_key;
  surface.typed_handoff_advanced_edge_compatibility_shard1_key =
      typed_surface.typed_advanced_edge_compatibility_shard1_key;
  surface.parse_artifact_advanced_diagnostics_shard1_key =
      parse_surface.toolchain_runtime_ga_operations_advanced_diagnostics_key;
  surface.typed_handoff_advanced_diagnostics_shard1_key =
      typed_surface.typed_advanced_diagnostics_shard1_key;
  surface.parse_artifact_advanced_conformance_shard1_key =
      parse_surface.toolchain_runtime_ga_operations_advanced_conformance_key;
  surface.typed_handoff_advanced_conformance_shard1_key =
      typed_surface.typed_advanced_conformance_shard1_key;
  surface.parse_artifact_advanced_integration_shard1_key =
      parse_surface.toolchain_runtime_ga_operations_advanced_integration_key;
  surface.typed_handoff_advanced_integration_shard1_key =
      typed_surface.typed_advanced_integration_shard1_key;
  surface.parse_artifact_edge_case_expansion_key =
      parse_surface.long_tail_grammar_expansion_key;
  surface.parse_artifact_edge_robustness_key =
      parse_surface.parse_artifact_edge_robustness_key;
}

}  // namespace objc3_ir_emission_core_feature_surface
