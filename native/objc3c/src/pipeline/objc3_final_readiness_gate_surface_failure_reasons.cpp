#include "pipeline/objc3_final_readiness_gate_surface_owners.h"
#include "pipeline/readiness/objc3_final_readiness_gate_failure_reasons.h"

namespace objc3_final_readiness_gate_surface {

void PublishObjc3FinalReadinessGateSurfaceFailureReasons(
    Objc3FinalReadinessGateCoreFeatureImplementationSurface &surface,
    const Objc3FinalReadinessGateLaneSurface &lane_a_surface,
    const Objc3FinalReadinessGateLaneSurface &lane_b_surface,
    const Objc3FinalReadinessGateLaneSurface &lane_c_surface,
    const Objc3FinalReadinessGateLaneSurface &lane_d_surface) {
  Objc3FinalReadinessGateFailureReasonInputs inputs;
  inputs.replay_keys_ready =
      !surface.governance_key.empty() &&
      !surface.modular_split_key.empty() &&
      !surface.lane_a_key.empty() &&
      !surface.lane_b_key.empty() &&
      !surface.lane_c_key.empty() &&
      !surface.lane_d_key.empty();
  inputs.lane_expansion_consistent =
      lane_a_surface.expansion_ready &&
      lane_b_surface.expansion_ready &&
      lane_c_surface.expansion_ready &&
      lane_d_surface.core_feature_expansion_ready;
  inputs.lane_edge_case_compatibility_consistent =
      lane_a_surface.edge_case_compatibility_ready &&
      lane_b_surface.edge_case_compatibility_ready &&
      lane_c_surface.edge_case_compatibility_ready &&
      lane_d_surface.edge_case_compatibility_ready;
  inputs.lane_edge_case_expansion_consistent =
      lane_a_surface.core_feature_ready &&
      lane_b_surface.core_feature_impl_ready &&
      lane_c_surface.core_feature_impl_ready &&
      lane_d_surface.edge_case_compatibility_ready;
  inputs.lane_diagnostics_hardening_consistent =
      lane_a_surface.core_feature_ready &&
      lane_b_surface.core_feature_impl_ready &&
      lane_c_surface.core_feature_impl_ready &&
      lane_d_surface.edge_case_robustness_ready;
  inputs.lane_recovery_determinism_consistent =
      lane_a_surface.core_feature_ready &&
      lane_b_surface.expansion_ready &&
      lane_c_surface.expansion_ready &&
      lane_d_surface.diagnostics_hardening_ready;
  inputs.lane_conformance_matrix_consistent =
      lane_a_surface.core_feature_ready &&
      lane_b_surface.expansion_ready &&
      lane_c_surface.expansion_ready &&
      lane_d_surface.diagnostics_hardening_ready;
  inputs.lane_conformance_corpus_consistent =
      lane_a_surface.core_feature_expansion_ready &&
      lane_b_surface.expansion_ready &&
      lane_c_surface.edge_case_compatibility_ready &&
      lane_d_surface.recovery_determinism_ready &&
      !lane_d_surface.recovery_determinism_key.empty();
  inputs.lane_performance_quality_guardrails_consistent =
      lane_a_surface.core_feature_expansion_ready &&
      lane_b_surface.edge_case_compatibility_ready &&
      lane_c_surface.edge_case_compatibility_ready &&
      lane_d_surface.conformance_matrix_ready &&
      !lane_d_surface.conformance_matrix_key.empty();
  inputs.lane_cross_lane_integration_consistent =
      lane_a_surface.core_feature_expansion_ready &&
      lane_b_surface.edge_case_compatibility_ready &&
      lane_c_surface.edge_case_robustness_ready &&
      lane_d_surface.conformance_matrix_ready &&
      !lane_d_surface.conformance_matrix_key.empty();
  inputs.lane_docs_runbook_sync_consistent =
      lane_a_surface.edge_case_compatibility_ready &&
      lane_b_surface.edge_case_robustness_ready &&
      lane_c_surface.edge_case_robustness_ready &&
      lane_d_surface.conformance_matrix_ready &&
      !lane_d_surface.conformance_matrix_key.empty();
  inputs.lane_release_candidate_replay_dry_run_consistent =
      lane_a_surface.edge_case_compatibility_ready &&
      lane_b_surface.edge_case_robustness_ready &&
      lane_c_surface.diagnostics_hardening_ready &&
      lane_d_surface.core_feature_impl_ready &&
      !lane_d_surface.core_feature_key.empty();
  inputs.lane_advanced_core_shard1_consistent =
      lane_a_surface.edge_case_robustness_ready &&
      lane_b_surface.diagnostics_hardening_ready &&
      lane_c_surface.diagnostics_hardening_ready &&
      lane_d_surface.core_feature_impl_ready &&
      !lane_d_surface.core_feature_key.empty();
  inputs.lane_advanced_edge_compatibility_shard1_consistent =
      lane_a_surface.edge_case_robustness_ready &&
      lane_b_surface.diagnostics_hardening_ready &&
      lane_c_surface.recovery_determinism_ready &&
      lane_d_surface.core_feature_impl_ready &&
      !lane_d_surface.core_feature_key.empty();
  inputs.lane_advanced_diagnostics_shard1_consistent =
      lane_a_surface.edge_case_robustness_ready &&
      lane_b_surface.recovery_determinism_ready &&
      lane_c_surface.recovery_determinism_ready &&
      lane_d_surface.core_feature_impl_ready &&
      !lane_d_surface.core_feature_key.empty();
  inputs.lane_advanced_conformance_shard1_consistent =
      lane_a_surface.diagnostics_hardening_ready &&
      lane_b_surface.recovery_determinism_ready &&
      lane_c_surface.conformance_matrix_ready &&
      lane_d_surface.core_feature_impl_ready &&
      !lane_d_surface.core_feature_key.empty();
  inputs.lane_advanced_integration_shard1_consistent =
      lane_a_surface.diagnostics_hardening_ready &&
      lane_b_surface.recovery_determinism_ready &&
      lane_c_surface.conformance_matrix_ready &&
      lane_d_surface.core_feature_impl_ready &&
      !lane_d_surface.core_feature_key.empty();
  inputs.lane_advanced_performance_shard1_consistent =
      lane_a_surface.diagnostics_hardening_ready &&
      lane_b_surface.conformance_matrix_ready &&
      lane_c_surface.conformance_corpus_ready &&
      lane_d_surface.edge_case_robustness_ready &&
      !lane_d_surface.edge_case_robustness_key.empty();
  inputs.lane_advanced_core_shard2_consistent =
      lane_a_surface.recovery_determinism_ready &&
      lane_b_surface.conformance_matrix_ready &&
      lane_c_surface.conformance_corpus_ready &&
      lane_d_surface.diagnostics_hardening_ready &&
      !lane_d_surface.diagnostics_hardening_key.empty();
  inputs.lane_advanced_core_shard3_consistent =
      lane_a_surface.conformance_corpus_ready &&
      lane_b_surface.cross_lane_integration_ready &&
      lane_c_surface.advanced_core_shard1_ready &&
      lane_d_surface.advanced_integration_shard1_ready &&
      !lane_d_surface.advanced_integration_shard1_key.empty();
  inputs.lane_advanced_edge_compatibility_shard3_consistent =
      lane_a_surface.conformance_corpus_ready &&
      lane_b_surface.docs_runbook_sync_ready &&
      lane_c_surface.advanced_core_shard1_ready &&
      lane_d_surface.advanced_performance_shard1_ready &&
      !lane_d_surface.advanced_performance_shard1_key.empty();
  inputs.lane_advanced_diagnostics_shard3_consistent =
      lane_a_surface.performance_quality_guardrails_ready &&
      lane_b_surface.docs_runbook_sync_ready &&
      lane_c_surface.advanced_edge_compatibility_shard1_ready &&
      lane_d_surface.advanced_core_shard2_ready &&
      !lane_d_surface.advanced_core_shard2_key.empty();
  inputs.lane_advanced_conformance_shard3_consistent =
      lane_a_surface.performance_quality_guardrails_ready &&
      lane_b_surface.release_candidate_replay_dry_run_ready &&
      lane_c_surface.advanced_edge_compatibility_shard1_ready &&
      lane_d_surface.advanced_core_shard2_ready &&
      !lane_d_surface.advanced_core_shard2_key.empty();
  inputs.lane_advanced_integration_shard3_consistent =
      lane_a_surface.cross_lane_integration_ready &&
      lane_b_surface.release_candidate_replay_dry_run_ready &&
      lane_c_surface.advanced_diagnostics_shard1_ready &&
      lane_d_surface.advanced_edge_compatibility_shard2_ready &&
      !lane_d_surface.advanced_edge_compatibility_shard2_key.empty();
  inputs.lane_advanced_performance_shard3_consistent =
      lane_a_surface.cross_lane_integration_ready &&
      lane_b_surface.advanced_core_shard1_ready &&
      lane_c_surface.advanced_diagnostics_shard1_ready &&
      lane_d_surface.advanced_diagnostics_shard2_ready &&
      !lane_d_surface.advanced_diagnostics_shard2_key.empty();
  inputs.lane_advanced_core_shard4_consistent =
      lane_a_surface.cross_lane_integration_ready &&
      lane_b_surface.advanced_core_shard1_ready &&
      lane_c_surface.advanced_conformance_shard1_ready &&
      lane_d_surface.advanced_conformance_shard2_ready &&
      !lane_d_surface.advanced_conformance_shard2_key.empty();
  inputs.lane_advanced_edge_compatibility_shard4_consistent =
      lane_a_surface.integration_closeout_signoff_ready &&
      lane_b_surface.integration_closeout_signoff_ready &&
      lane_c_surface.advanced_conformance_shard1_ready &&
      lane_d_surface.advanced_conformance_shard2_ready &&
      !lane_d_surface.advanced_conformance_shard2_key.empty();
  inputs.lane_advanced_integration_closeout_signoff_consistent =
      lane_a_surface.integration_closeout_signoff_ready &&
      lane_b_surface.integration_closeout_signoff_ready &&
      lane_c_surface.integration_closeout_signoff_ready &&
      lane_d_surface.integration_closeout_signoff_ready &&
      !lane_d_surface.integration_closeout_signoff_key.empty();
  inputs.lane_advanced_edge_compatibility_shard2_consistent =
      lane_a_surface.recovery_determinism_ready &&
      lane_b_surface.conformance_corpus_ready &&
      lane_c_surface.performance_quality_guardrails_ready &&
      lane_d_surface.conformance_matrix_ready &&
      !lane_d_surface.conformance_matrix_key.empty();
  inputs.lane_advanced_diagnostics_shard2_consistent =
      lane_a_surface.conformance_matrix_ready &&
      lane_b_surface.conformance_corpus_ready &&
      lane_c_surface.performance_quality_guardrails_ready &&
      lane_d_surface.core_feature_impl_ready &&
      !lane_d_surface.core_feature_key.empty();
  inputs.lane_advanced_conformance_shard2_consistent =
      lane_a_surface.conformance_matrix_ready &&
      lane_b_surface.performance_quality_guardrails_ready &&
      lane_c_surface.cross_lane_integration_ready &&
      lane_d_surface.core_feature_impl_ready &&
      !lane_d_surface.core_feature_key.empty();
  inputs.lane_advanced_integration_shard2_consistent =
      lane_a_surface.conformance_matrix_ready &&
      lane_b_surface.performance_quality_guardrails_ready &&
      lane_c_surface.cross_lane_integration_ready &&
      lane_d_surface.edge_case_robustness_ready &&
      !lane_d_surface.edge_case_robustness_key.empty();
  inputs.lane_advanced_performance_shard2_consistent =
      lane_a_surface.conformance_matrix_ready &&
      lane_b_surface.performance_quality_guardrails_ready &&
      lane_c_surface.cross_lane_integration_ready &&
      lane_d_surface.diagnostics_hardening_ready &&
      !lane_d_surface.diagnostics_hardening_key.empty();
  inputs.lane_integration_closeout_signoff_consistent =
      lane_a_surface.conformance_matrix_ready &&
      lane_b_surface.performance_quality_guardrails_ready &&
      lane_c_surface.cross_lane_integration_ready &&
      lane_d_surface.conformance_matrix_ready &&
      !lane_d_surface.conformance_matrix_key.empty();

  ApplyObjc3FinalReadinessGateFailureReason(surface, inputs);
}

}  // namespace objc3_final_readiness_gate_surface
