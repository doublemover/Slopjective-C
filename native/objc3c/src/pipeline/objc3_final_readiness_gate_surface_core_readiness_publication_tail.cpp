#include "pipeline/objc3_final_readiness_gate_surface_core_readiness_publication_tail.h"

#include <string>

#include "pipeline/readiness/objc3_final_readiness_gate_core_keys.h"

namespace objc3_final_readiness_gate_surface {

void PublishObjc3FinalReadinessGateTailReadiness(
    Objc3FinalReadinessGateCoreFeatureImplementationSurface &surface,
    const Objc3FinalReadinessGateLaneSurface &lane_a_surface,
    const Objc3FinalReadinessGateLaneSurface &lane_b_surface,
    const Objc3FinalReadinessGateLaneSurface &lane_c_surface,
    const Objc3FinalReadinessGateLaneSurface &lane_d_surface) {
  const bool lane_conformance_corpus_consistent =
      lane_a_surface.expansion_ready &&
      lane_b_surface.expansion_ready &&
      lane_c_surface.edge_case_compatibility_ready &&
      lane_d_surface.recovery_determinism_ready &&
      !lane_d_surface.recovery_determinism_key.empty();
  const bool conformance_corpus_consistent =
      surface.conformance_matrix_ready &&
      lane_conformance_corpus_consistent;
  const bool conformance_corpus_ready =
      conformance_corpus_consistent &&
      !surface.governance_key.empty() &&
      !surface.modular_split_key.empty() &&
      !surface.conformance_matrix_key.empty() &&
      !lane_d_surface.recovery_determinism_key.empty();
  surface.conformance_corpus_consistent =
      conformance_corpus_consistent;
  surface.conformance_corpus_ready =
      conformance_corpus_ready;
  surface.conformance_corpus_key =
      BuildObjc3FinalReadinessGateConformanceCorpusKey(
          surface,
          lane_a_surface.expansion_ready,
          lane_b_surface.expansion_ready,
          lane_c_surface.edge_case_compatibility_ready,
          lane_d_surface.recovery_determinism_ready,
          !lane_d_surface.recovery_determinism_key.empty());
  surface.conformance_corpus_ready =
      surface.conformance_corpus_ready &&
      !surface.conformance_corpus_key.empty();
  const bool lane_performance_quality_guardrails_consistent =
      lane_a_surface.expansion_ready &&
      lane_b_surface.edge_case_compatibility_ready &&
      lane_c_surface.edge_case_compatibility_ready &&
      lane_d_surface.conformance_matrix_ready &&
      !lane_d_surface.conformance_matrix_key.empty();
  const bool performance_quality_guardrails_consistent =
      surface.conformance_corpus_ready &&
      lane_performance_quality_guardrails_consistent;
  const bool performance_quality_guardrails_ready =
      performance_quality_guardrails_consistent &&
      !surface.governance_key.empty() &&
      !surface.modular_split_key.empty() &&
      !surface.conformance_corpus_key.empty() &&
      !lane_d_surface.conformance_matrix_key.empty();
  surface.performance_quality_guardrails_consistent =
      performance_quality_guardrails_consistent;
  surface.performance_quality_guardrails_ready =
      performance_quality_guardrails_ready;
  surface.performance_quality_guardrails_key =
      BuildObjc3FinalReadinessGatePerformanceQualityGuardrailsKey(
          surface,
          lane_a_surface.expansion_ready,
          lane_b_surface.edge_case_compatibility_ready,
          lane_c_surface.edge_case_compatibility_ready,
          lane_d_surface.conformance_matrix_ready,
          !lane_d_surface.conformance_matrix_key.empty());
  surface.performance_quality_guardrails_ready =
      surface.performance_quality_guardrails_ready &&
      !surface.performance_quality_guardrails_key.empty();
  const bool lane_cross_lane_integration_consistent =
      lane_a_surface.expansion_ready &&
      lane_b_surface.edge_case_compatibility_ready &&
      lane_c_surface.edge_case_robustness_ready &&
      lane_d_surface.conformance_matrix_ready &&
      !lane_d_surface.conformance_matrix_key.empty();
  const bool cross_lane_integration_consistent =
      surface.performance_quality_guardrails_ready &&
      lane_cross_lane_integration_consistent;
  const bool cross_lane_integration_ready =
      cross_lane_integration_consistent &&
      !surface.governance_key.empty() &&
      !surface.modular_split_key.empty() &&
      !surface.performance_quality_guardrails_key.empty() &&
      !lane_d_surface.conformance_matrix_key.empty();
  surface.cross_lane_integration_consistent =
      cross_lane_integration_consistent;
  surface.cross_lane_integration_ready =
      cross_lane_integration_ready;
  surface.cross_lane_integration_key =
      BuildObjc3FinalReadinessGateCrossLaneIntegrationKey(
          surface,
          lane_a_surface.expansion_ready,
          lane_b_surface.edge_case_compatibility_ready,
          lane_c_surface.edge_case_robustness_ready,
          lane_d_surface.conformance_matrix_ready,
          !lane_d_surface.conformance_matrix_key.empty());
  surface.cross_lane_integration_ready =
      surface.cross_lane_integration_ready &&
      !surface.cross_lane_integration_key.empty();
  const bool lane_docs_runbook_sync_consistent =
      lane_a_surface.edge_case_compatibility_ready &&
      lane_b_surface.edge_case_robustness_ready &&
      lane_c_surface.edge_case_robustness_ready &&
      lane_d_surface.conformance_matrix_ready &&
      !lane_d_surface.conformance_matrix_key.empty();
  const bool docs_runbook_sync_consistent =
      surface.cross_lane_integration_ready &&
      lane_docs_runbook_sync_consistent;
  const bool docs_runbook_sync_ready =
      docs_runbook_sync_consistent &&
      !surface.governance_key.empty() &&
      !surface.modular_split_key.empty() &&
      !surface.cross_lane_integration_key.empty() &&
      !lane_d_surface.conformance_matrix_key.empty();
  surface.docs_runbook_sync_consistent =
      docs_runbook_sync_consistent;
  surface.docs_runbook_sync_ready =
      docs_runbook_sync_ready;
  surface.docs_runbook_sync_key =
      BuildObjc3FinalReadinessGateDocsRunbookSyncKey(
          surface,
          lane_a_surface.edge_case_compatibility_ready,
          lane_b_surface.edge_case_robustness_ready,
          lane_c_surface.edge_case_robustness_ready,
          lane_d_surface.conformance_matrix_ready,
          !lane_d_surface.conformance_matrix_key.empty());
  surface.docs_runbook_sync_ready =
      surface.docs_runbook_sync_ready &&
      !surface.docs_runbook_sync_key.empty();
  const bool lane_release_candidate_replay_dry_run_consistent =
      lane_a_surface.edge_case_compatibility_ready &&
      lane_b_surface.edge_case_robustness_ready &&
      lane_c_surface.diagnostics_hardening_ready &&
      lane_d_surface.core_feature_impl_ready &&
      !lane_d_surface.core_feature_key.empty();
  const bool release_candidate_replay_dry_run_consistent =
      surface.docs_runbook_sync_ready &&
      lane_release_candidate_replay_dry_run_consistent;
  const bool release_candidate_replay_dry_run_ready =
      release_candidate_replay_dry_run_consistent &&
      !surface.governance_key.empty() &&
      !surface.modular_split_key.empty() &&
      !surface.docs_runbook_sync_key.empty() &&
      !lane_d_surface.core_feature_key.empty();
  surface.release_candidate_replay_dry_run_consistent =
      release_candidate_replay_dry_run_consistent;
  surface.release_candidate_replay_dry_run_ready =
      release_candidate_replay_dry_run_ready;
  surface.release_candidate_replay_dry_run_key =
      BuildObjc3FinalReadinessGateReleaseCandidateReplayDryRunKey(
          surface,
          lane_a_surface.edge_case_compatibility_ready,
          lane_b_surface.edge_case_robustness_ready,
          lane_c_surface.diagnostics_hardening_ready,
          lane_d_surface.core_feature_impl_ready,
          !lane_d_surface.core_feature_key.empty());
  surface.release_candidate_replay_dry_run_ready =
      surface.release_candidate_replay_dry_run_ready &&
      !surface.release_candidate_replay_dry_run_key.empty();
}

}  // namespace objc3_final_readiness_gate_surface
