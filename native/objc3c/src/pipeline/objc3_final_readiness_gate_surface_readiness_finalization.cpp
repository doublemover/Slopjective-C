#include "pipeline/objc3_final_readiness_gate_surface_readiness_helpers.h"
#include "pipeline/readiness/objc3_final_readiness_gate_core_keys.h"

namespace objc3_final_readiness_gate_surface {

void FinalizeObjc3FinalReadinessGateSurfaceReadiness(
    Objc3FinalReadinessGateCoreFeatureImplementationSurface &surface) {
  surface.core_feature_key =
      BuildObjc3FinalReadinessGateCoreFeatureImplementationKey(surface);
  surface.core_feature_impl_ready =
      surface.core_feature_impl_ready &&
      surface.core_feature_expansion_ready &&
      surface.edge_case_compatibility_ready &&
      surface.edge_case_robustness_ready &&
      surface.diagnostics_hardening_ready &&
      surface.recovery_determinism_ready &&
      surface.conformance_matrix_ready &&
      surface.conformance_corpus_ready &&
      surface.performance_quality_guardrails_ready &&
      surface.cross_lane_integration_ready &&
      surface.docs_runbook_sync_ready &&
      surface.release_candidate_replay_dry_run_ready &&
      surface.advanced_core_shard1_ready &&
      surface.advanced_edge_compatibility_shard1_ready &&
      surface.advanced_diagnostics_shard1_ready &&
      surface.advanced_conformance_shard1_ready &&
      surface.advanced_integration_shard1_ready &&
      surface.advanced_performance_shard1_ready &&
      surface.advanced_core_shard2_ready &&
      surface.advanced_core_shard3_ready &&
      surface.advanced_edge_compatibility_shard3_ready &&
      surface.advanced_diagnostics_shard3_ready &&
      surface.advanced_conformance_shard3_ready &&
      surface.advanced_integration_shard3_ready &&
      surface.advanced_performance_shard3_ready &&
      surface.advanced_core_shard4_ready &&
      surface.advanced_edge_compatibility_shard4_ready &&
      surface.advanced_integration_closeout_signoff_ready &&
      surface.advanced_edge_compatibility_shard2_ready &&
      surface.advanced_diagnostics_shard2_ready &&
      surface.advanced_conformance_shard2_ready &&
      surface.advanced_integration_shard2_ready &&
      surface.advanced_performance_shard2_ready &&
      surface.integration_closeout_signoff_ready &&
      !surface.core_feature_key.empty();
}

}  // namespace objc3_final_readiness_gate_surface
