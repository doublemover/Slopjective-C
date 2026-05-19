#include "artifacts/objc3_frontend_artifact_pipeline_readiness_metadata.h"

#include "ir/objc3_ir_frontend_metadata.h"
#include "lower/model/lowered_module_pipeline_surfaces.h"
#include "pipeline/objc3_ir_emission_core_feature_implementation_surface.h"
#include "pipeline/objc3_ownership_aware_lowering_behavior_scaffold.h"

namespace objc3::artifacts::frontend {

void ApplyObjc3FrontendPipelineReadinessMetadata(
    Objc3IRFrontendMetadata &ir_frontend_metadata,
    const Objc3OwnershipAwareLoweringBehaviorScaffold
        &ownership_aware_lowering_behavior_scaffold,
    const Objc3IREmissionCompletenessScaffold
        &ir_emission_completeness_scaffold,
    const Objc3LoweringPipelinePassGraphCoreFeatureSurface
        &lowering_pipeline_pass_graph_core_feature_surface,
    const Objc3IREmissionCoreFeatureImplementationSurface
        &ir_emission_core_feature_impl_surface) {
  ir_frontend_metadata.ownership_aware_lowering_core_feature_expansion_ready =
      ownership_aware_lowering_behavior_scaffold.expansion_ready;
  ir_frontend_metadata.ownership_aware_lowering_core_feature_expansion_key =
      ownership_aware_lowering_behavior_scaffold.expansion_key;
  ir_frontend_metadata
      .ownership_aware_lowering_performance_quality_guardrails_ready =
      ownership_aware_lowering_behavior_scaffold
          .performance_quality_guardrails_ready;
  ir_frontend_metadata
      .ownership_aware_lowering_performance_quality_guardrails_key =
      ownership_aware_lowering_behavior_scaffold
          .performance_quality_guardrails_key;
  ir_frontend_metadata.ownership_aware_lowering_cross_lane_integration_ready =
      ownership_aware_lowering_behavior_scaffold.cross_lane_integration_ready;
  ir_frontend_metadata.ownership_aware_lowering_cross_lane_integration_key =
      ownership_aware_lowering_behavior_scaffold.cross_lane_integration_key;

  ir_frontend_metadata.lowering_pass_graph_core_feature_ready =
      ir_emission_completeness_scaffold.core_feature_ready;
  ir_frontend_metadata.lowering_pass_graph_core_feature_key =
      ir_emission_completeness_scaffold.core_feature_key;
  ir_frontend_metadata.lowering_pass_graph_core_feature_expansion_ready =
      ir_emission_completeness_scaffold.expansion_ready;
  ir_frontend_metadata.lowering_pass_graph_core_feature_expansion_key =
      ir_emission_completeness_scaffold.expansion_key;
  ir_frontend_metadata.lowering_pass_graph_edge_case_compatibility_ready =
      ir_emission_completeness_scaffold.edge_case_compatibility_ready;
  ir_frontend_metadata.lowering_pass_graph_edge_case_compatibility_key =
      ir_emission_completeness_scaffold.edge_case_compatibility_key;
  ir_frontend_metadata.lowering_pass_graph_edge_case_robustness_ready =
      lowering_pipeline_pass_graph_core_feature_surface
          .edge_case_robustness_ready;
  ir_frontend_metadata.lowering_pass_graph_edge_case_robustness_key =
      lowering_pipeline_pass_graph_core_feature_surface
          .edge_case_robustness_key;
  ir_frontend_metadata.lowering_pass_graph_diagnostics_hardening_ready =
      lowering_pipeline_pass_graph_core_feature_surface
          .diagnostics_hardening_ready;
  ir_frontend_metadata.lowering_pass_graph_diagnostics_hardening_key =
      lowering_pipeline_pass_graph_core_feature_surface
          .diagnostics_hardening_key;
  ir_frontend_metadata.lowering_pass_graph_recovery_determinism_ready =
      lowering_pipeline_pass_graph_core_feature_surface
          .recovery_determinism_ready;
  ir_frontend_metadata.lowering_pass_graph_recovery_determinism_key =
      lowering_pipeline_pass_graph_core_feature_surface
          .recovery_determinism_key;
  ir_frontend_metadata.lowering_pass_graph_conformance_matrix_ready =
      lowering_pipeline_pass_graph_core_feature_surface.conformance_matrix_ready;
  ir_frontend_metadata.lowering_pass_graph_conformance_matrix_key =
      lowering_pipeline_pass_graph_core_feature_surface.conformance_matrix_key;
  ir_frontend_metadata.lowering_pass_graph_conformance_corpus_ready =
      lowering_pipeline_pass_graph_core_feature_surface.conformance_corpus_ready;
  ir_frontend_metadata.lowering_pass_graph_conformance_corpus_key =
      lowering_pipeline_pass_graph_core_feature_surface.conformance_corpus_key;
  ir_frontend_metadata.lowering_pass_graph_performance_quality_guardrails_ready =
      lowering_pipeline_pass_graph_core_feature_surface
          .performance_quality_guardrails_ready;
  ir_frontend_metadata.lowering_pass_graph_performance_quality_guardrails_key =
      lowering_pipeline_pass_graph_core_feature_surface
          .performance_quality_guardrails_key;

  ir_frontend_metadata.ir_emission_completeness_modular_split_ready =
      ir_emission_completeness_scaffold.modular_split_ready;
  ir_frontend_metadata.ir_emission_completeness_modular_split_key =
      ir_emission_completeness_scaffold.scaffold_key;
  ir_frontend_metadata.ir_emission_core_feature_impl_ready =
      ir_emission_core_feature_impl_surface.core_feature_impl_ready;
  ir_frontend_metadata.ir_emission_core_feature_impl_key =
      ir_emission_core_feature_impl_surface.core_feature_key;
  ir_frontend_metadata.ir_emission_core_feature_expansion_ready =
      ir_emission_core_feature_impl_surface.core_feature_expansion_ready;
  ir_frontend_metadata.ir_emission_core_feature_expansion_key =
      ir_emission_core_feature_impl_surface.expansion_key;
  ir_frontend_metadata.ir_emission_core_feature_edge_case_compatibility_ready =
      ir_emission_core_feature_impl_surface
          .core_feature_edge_case_compatibility_ready;
  ir_frontend_metadata.ir_emission_core_feature_edge_case_compatibility_key =
      ir_emission_core_feature_impl_surface.edge_case_compatibility_key;
  ir_frontend_metadata.ir_emission_core_feature_edge_case_robustness_ready =
      ir_emission_core_feature_impl_surface
          .core_feature_edge_case_robustness_ready;
  ir_frontend_metadata.ir_emission_core_feature_edge_case_robustness_key =
      ir_emission_core_feature_impl_surface.edge_case_robustness_key;
  ir_frontend_metadata.ir_emission_core_feature_diagnostics_hardening_ready =
      ir_emission_core_feature_impl_surface
          .core_feature_diagnostics_hardening_ready;
  ir_frontend_metadata.ir_emission_core_feature_diagnostics_hardening_key =
      ir_emission_core_feature_impl_surface.diagnostics_hardening_key;
  ir_frontend_metadata.ir_emission_core_feature_recovery_determinism_ready =
      ir_emission_core_feature_impl_surface
          .core_feature_recovery_determinism_ready;
  ir_frontend_metadata.ir_emission_core_feature_recovery_determinism_key =
      ir_emission_core_feature_impl_surface.recovery_determinism_key;
  ir_frontend_metadata.ir_emission_core_feature_conformance_matrix_ready =
      ir_emission_core_feature_impl_surface
          .core_feature_conformance_matrix_ready;
  ir_frontend_metadata.ir_emission_core_feature_conformance_matrix_key =
      ir_emission_core_feature_impl_surface.conformance_matrix_key;
  ir_frontend_metadata.ir_emission_core_feature_conformance_corpus_ready =
      ir_emission_core_feature_impl_surface
          .core_feature_conformance_corpus_ready;
  ir_frontend_metadata.ir_emission_core_feature_conformance_corpus_key =
      ir_emission_core_feature_impl_surface.conformance_corpus_key;
  ir_frontend_metadata
      .ir_emission_core_feature_performance_quality_guardrails_ready =
      ir_emission_core_feature_impl_surface
          .core_feature_performance_quality_guardrails_ready;
  ir_frontend_metadata
      .ir_emission_core_feature_performance_quality_guardrails_key =
      ir_emission_core_feature_impl_surface.performance_quality_guardrails_key;
  ir_frontend_metadata
      .ir_emission_core_feature_cross_lane_integration_sync_ready =
      ir_emission_core_feature_impl_surface
          .core_feature_cross_lane_integration_sync_ready;
  ir_frontend_metadata.ir_emission_core_feature_cross_lane_integration_sync_key =
      ir_emission_core_feature_impl_surface.cross_lane_integration_sync_key;
  ir_frontend_metadata.ir_emission_core_feature_advanced_core_shard1_ready =
      ir_emission_core_feature_impl_surface
          .core_feature_advanced_core_shard1_ready;
  ir_frontend_metadata.ir_emission_core_feature_advanced_core_shard1_key =
      ir_emission_core_feature_impl_surface.advanced_core_shard1_key;
  ir_frontend_metadata
      .ir_emission_core_feature_advanced_edge_compatibility_shard1_ready =
      ir_emission_core_feature_impl_surface
          .core_feature_advanced_edge_compatibility_shard1_ready;
  ir_frontend_metadata
      .ir_emission_core_feature_advanced_edge_compatibility_shard1_key =
      ir_emission_core_feature_impl_surface
          .advanced_edge_compatibility_shard1_key;
  ir_frontend_metadata
      .ir_emission_core_feature_advanced_diagnostics_shard1_ready =
      ir_emission_core_feature_impl_surface
          .core_feature_advanced_diagnostics_shard1_ready;
  ir_frontend_metadata.ir_emission_core_feature_advanced_diagnostics_shard1_key =
      ir_emission_core_feature_impl_surface.advanced_diagnostics_shard1_key;
  ir_frontend_metadata.ir_emission_core_feature_advanced_conformance_shard1_ready =
      ir_emission_core_feature_impl_surface
          .core_feature_advanced_conformance_shard1_ready;
  ir_frontend_metadata.ir_emission_core_feature_advanced_conformance_shard1_key =
      ir_emission_core_feature_impl_surface.advanced_conformance_shard1_key;
  ir_frontend_metadata.ir_emission_core_feature_advanced_integration_shard1_ready =
      ir_emission_core_feature_impl_surface
          .core_feature_advanced_integration_shard1_ready;
  ir_frontend_metadata.ir_emission_core_feature_advanced_integration_shard1_key =
      ir_emission_core_feature_impl_surface.advanced_integration_shard1_key;
}

}  // namespace objc3::artifacts::frontend
