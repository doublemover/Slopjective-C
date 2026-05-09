#include "pipeline/readiness/objc3_final_readiness_gate_failure_reasons.h"

#include <string>

void ApplyObjc3FinalReadinessGateFailureReason(
    Objc3FinalReadinessGateCoreFeatureImplementationSurface &surface,
    const Objc3FinalReadinessGateFailureReasonInputs &inputs) {
  if (surface.core_feature_impl_ready) {
    return;
  }

  if (!surface.governance_contract_ready) {
    surface.failure_reason =
        "final readiness gate governance contract is not ready";
  } else if (!surface.modular_split_ready) {
    surface.failure_reason =
        "final readiness gate modular split scaffold is not ready";
  } else if (!surface.lane_a_core_feature_ready) {
    surface.failure_reason =
        "final readiness gate lane-A core feature readiness is not satisfied";
  } else if (!surface.lane_b_core_feature_ready) {
    surface.failure_reason =
        "final readiness gate lane-B core feature readiness is not satisfied";
  } else if (!surface.lane_c_core_feature_ready) {
    surface.failure_reason =
        "final readiness gate lane-C core feature readiness is not satisfied";
  } else if (!surface.lane_d_core_feature_ready) {
    surface.failure_reason =
        "final readiness gate lane-D core feature readiness is not satisfied";
  } else if (!inputs.replay_keys_ready) {
    surface.failure_reason =
        "final readiness gate dependency replay keys are not ready";
  } else if (!inputs.lane_expansion_consistent) {
    surface.failure_reason =
        "final readiness gate core feature expansion is inconsistent";
  } else if (!surface.core_feature_expansion_consistent) {
    surface.failure_reason =
        "final readiness gate core feature expansion consistency is not satisfied";
  } else if (!surface.core_feature_expansion_ready) {
    surface.failure_reason =
        "final readiness gate core feature expansion is not ready";
  } else if (surface.core_feature_expansion_key.empty()) {
    surface.failure_reason =
        "final readiness gate core feature expansion key is not ready";
  } else if (!inputs.lane_edge_case_compatibility_consistent) {
    surface.failure_reason =
        "final readiness gate edge-case compatibility is inconsistent";
  } else if (!surface.edge_case_compatibility_consistent) {
    surface.failure_reason =
        "final readiness gate edge-case compatibility consistency is not satisfied";
  } else if (!surface.edge_case_compatibility_ready) {
    surface.failure_reason =
        "final readiness gate edge-case compatibility is not ready";
  } else if (surface.edge_case_compatibility_key.empty()) {
    surface.failure_reason =
        "final readiness gate edge-case compatibility key is not ready";
  } else if (!inputs.lane_edge_case_expansion_consistent) {
    surface.failure_reason =
        "final readiness gate edge-case expansion is inconsistent";
  } else if (!surface.edge_case_expansion_consistent) {
    surface.failure_reason =
        "final readiness gate edge-case expansion consistency is not satisfied";
  } else if (!surface.edge_case_robustness_ready) {
    surface.failure_reason =
        "final readiness gate edge-case robustness is not ready";
  } else if (surface.edge_case_robustness_key.empty()) {
    surface.failure_reason =
        "final readiness gate edge-case robustness key is not ready";
  } else if (!inputs.lane_diagnostics_hardening_consistent) {
    surface.failure_reason =
        "final readiness gate diagnostics hardening is inconsistent";
  } else if (!surface.diagnostics_hardening_consistent) {
    surface.failure_reason =
        "final readiness gate diagnostics hardening consistency is not satisfied";
  } else if (!surface.diagnostics_hardening_ready) {
    surface.failure_reason =
        "final readiness gate diagnostics hardening is not ready";
  } else if (surface.diagnostics_hardening_key.empty()) {
    surface.failure_reason =
        "final readiness gate diagnostics hardening key is not ready";
  } else if (!inputs.lane_recovery_determinism_consistent) {
    surface.failure_reason =
        "final readiness gate recovery and determinism hardening is inconsistent";
  } else if (!surface.recovery_determinism_consistent) {
    surface.failure_reason =
        "final readiness gate recovery and determinism consistency is not satisfied";
  } else if (!surface.recovery_determinism_ready) {
    surface.failure_reason =
        "final readiness gate recovery and determinism hardening is not ready";
  } else if (surface.recovery_determinism_key.empty()) {
    surface.failure_reason =
        "final readiness gate recovery and determinism key is not ready";
  } else if (!inputs.lane_conformance_matrix_consistent) {
    surface.failure_reason =
        "final readiness gate conformance matrix is inconsistent";
  } else if (!surface.conformance_matrix_consistent) {
    surface.failure_reason =
        "final readiness gate conformance matrix consistency is not satisfied";
  } else if (!surface.conformance_matrix_ready) {
    surface.failure_reason =
        "final readiness gate conformance matrix is not ready";
  } else if (surface.conformance_matrix_key.empty()) {
    surface.failure_reason =
        "final readiness gate conformance matrix key is not ready";
  } else if (!inputs.lane_conformance_corpus_consistent) {
    surface.failure_reason =
        "final readiness gate conformance corpus is inconsistent";
  } else if (!surface.conformance_corpus_consistent) {
    surface.failure_reason =
        "final readiness gate conformance corpus consistency is not satisfied";
  } else if (!surface.conformance_corpus_ready) {
    surface.failure_reason =
        "final readiness gate conformance corpus is not ready";
  } else if (surface.conformance_corpus_key.empty()) {
    surface.failure_reason =
        "final readiness gate conformance corpus key is not ready";
  } else if (!inputs.lane_performance_quality_guardrails_consistent) {
    surface.failure_reason =
        "final readiness gate performance and quality guardrails are inconsistent";
  } else if (!surface.performance_quality_guardrails_consistent) {
    surface.failure_reason =
        "final readiness gate performance and quality guardrails consistency is not satisfied";
  } else if (!surface.performance_quality_guardrails_ready) {
    surface.failure_reason =
        "final readiness gate performance and quality guardrails are not ready";
  } else if (surface.performance_quality_guardrails_key.empty()) {
    surface.failure_reason =
        "final readiness gate performance and quality guardrails key is not ready";
  } else if (!inputs.lane_cross_lane_integration_consistent) {
    surface.failure_reason =
        "final readiness gate cross-lane integration sync is inconsistent";
  } else if (!surface.cross_lane_integration_consistent) {
    surface.failure_reason =
        "final readiness gate cross-lane integration sync consistency is not satisfied";
  } else if (!surface.cross_lane_integration_ready) {
    surface.failure_reason =
        "final readiness gate cross-lane integration sync is not ready";
  } else if (surface.cross_lane_integration_key.empty()) {
    surface.failure_reason =
        "final readiness gate cross-lane integration sync key is not ready";
  } else if (!inputs.lane_docs_runbook_sync_consistent) {
    surface.failure_reason =
        "final readiness gate docs and operator runbook sync is inconsistent";
  } else if (!surface.docs_runbook_sync_consistent) {
    surface.failure_reason =
        "final readiness gate docs and operator runbook sync consistency is not satisfied";
  } else if (!surface.docs_runbook_sync_ready) {
    surface.failure_reason =
        "final readiness gate docs and operator runbook sync is not ready";
  } else if (surface.docs_runbook_sync_key.empty()) {
    surface.failure_reason =
        "final readiness gate docs and operator runbook sync key is not ready";
  } else if (!inputs.lane_release_candidate_replay_dry_run_consistent) {
    surface.failure_reason =
        "final readiness gate release candidate replay dry-run is inconsistent";
  } else if (!surface.release_candidate_replay_dry_run_consistent) {
    surface.failure_reason =
        "final readiness gate release candidate replay dry-run consistency is not satisfied";
  } else if (!surface.release_candidate_replay_dry_run_ready) {
    surface.failure_reason =
        "final readiness gate release candidate replay dry-run is not ready";
  } else if (surface.release_candidate_replay_dry_run_key.empty()) {
    surface.failure_reason =
        "final readiness gate release candidate replay dry-run key is not ready";
  } else if (!inputs.lane_advanced_core_shard1_consistent) {
    surface.failure_reason =
        "final readiness gate advanced core workpack shard1 is inconsistent";
  } else if (!surface.advanced_core_shard1_consistent) {
    surface.failure_reason =
        "final readiness gate advanced core workpack shard1 consistency is not satisfied";
  } else if (!surface.advanced_core_shard1_ready) {
    surface.failure_reason =
        "final readiness gate advanced core workpack shard1 is not ready";
  } else if (surface.advanced_core_shard1_key.empty()) {
    surface.failure_reason =
        "final readiness gate advanced core workpack shard1 key is not ready";
  } else if (!inputs.lane_advanced_edge_compatibility_shard1_consistent) {
    surface.failure_reason =
        "final readiness gate advanced edge compatibility workpack shard1 is inconsistent";
  } else if (!surface.advanced_edge_compatibility_shard1_consistent) {
    surface.failure_reason =
        "final readiness gate advanced edge compatibility workpack shard1 consistency is not satisfied";
  } else if (!surface.advanced_edge_compatibility_shard1_ready) {
    surface.failure_reason =
        "final readiness gate advanced edge compatibility workpack shard1 is not ready";
  } else if (surface.advanced_edge_compatibility_shard1_key.empty()) {
    surface.failure_reason =
        "final readiness gate advanced edge compatibility workpack shard1 key is not ready";
  } else if (!inputs.lane_advanced_diagnostics_shard1_consistent) {
    surface.failure_reason =
        "final readiness gate advanced diagnostics workpack shard1 is inconsistent";
  } else if (!surface.advanced_diagnostics_shard1_consistent) {
    surface.failure_reason =
        "final readiness gate advanced diagnostics workpack shard1 consistency is not satisfied";
  } else if (!surface.advanced_diagnostics_shard1_ready) {
    surface.failure_reason =
        "final readiness gate advanced diagnostics workpack shard1 is not ready";
  } else if (surface.advanced_diagnostics_shard1_key.empty()) {
    surface.failure_reason =
        "final readiness gate advanced diagnostics workpack shard1 key is not ready";
  } else if (!inputs.lane_advanced_conformance_shard1_consistent) {
    surface.failure_reason =
        "final readiness gate advanced conformance workpack shard1 is inconsistent";
  } else if (!surface.advanced_conformance_shard1_consistent) {
    surface.failure_reason =
        "final readiness gate advanced conformance workpack shard1 consistency is not satisfied";
  } else if (!surface.advanced_conformance_shard1_ready) {
    surface.failure_reason =
        "final readiness gate advanced conformance workpack shard1 is not ready";
  } else if (surface.advanced_conformance_shard1_key.empty()) {
    surface.failure_reason =
        "final readiness gate advanced conformance workpack shard1 key is not ready";
  } else if (!inputs.lane_advanced_integration_shard1_consistent) {
    surface.failure_reason =
        "final readiness gate advanced integration workpack shard1 is inconsistent";
  } else if (!surface.advanced_integration_shard1_consistent) {
    surface.failure_reason =
        "final readiness gate advanced integration workpack shard1 consistency is not satisfied";
  } else if (!surface.advanced_integration_shard1_ready) {
    surface.failure_reason =
        "final readiness gate advanced integration workpack shard1 is not ready";
  } else if (surface.advanced_integration_shard1_key.empty()) {
    surface.failure_reason =
        "final readiness gate advanced integration workpack shard1 key is not ready";
  } else if (!inputs.lane_advanced_performance_shard1_consistent) {
    surface.failure_reason =
        "final readiness gate advanced performance workpack shard1 is inconsistent";
  } else if (!surface.advanced_performance_shard1_consistent) {
    surface.failure_reason =
        "final readiness gate advanced performance workpack shard1 consistency is not satisfied";
  } else if (!surface.advanced_performance_shard1_ready) {
    surface.failure_reason =
        "final readiness gate advanced performance workpack shard1 is not ready";
  } else if (surface.advanced_performance_shard1_key.empty()) {
    surface.failure_reason =
        "final readiness gate advanced performance workpack shard1 key is not ready";
  } else if (!inputs.lane_advanced_core_shard2_consistent) {
    surface.failure_reason =
        "final readiness gate advanced core workpack shard2 is inconsistent";
  } else if (!surface.advanced_core_shard2_consistent) {
    surface.failure_reason =
        "final readiness gate advanced core workpack shard2 consistency is not satisfied";
  } else if (!surface.advanced_core_shard2_ready) {
    surface.failure_reason =
        "final readiness gate advanced core workpack shard2 is not ready";
  } else if (surface.advanced_core_shard2_key.empty()) {
    surface.failure_reason =
        "final readiness gate advanced core workpack shard2 key is not ready";
  } else if (!inputs.lane_advanced_core_shard3_consistent) {
    surface.failure_reason =
        "final readiness gate advanced core workpack shard3 is inconsistent";
  } else if (!surface.advanced_core_shard3_consistent) {
    surface.failure_reason =
        "final readiness gate advanced core workpack shard3 consistency is not satisfied";
  } else if (!surface.advanced_core_shard3_ready) {
    surface.failure_reason =
        "final readiness gate advanced core workpack shard3 is not ready";
  } else if (surface.advanced_core_shard3_key.empty()) {
    surface.failure_reason =
        "final readiness gate advanced core workpack shard3 key is not ready";
  } else if (!inputs.lane_advanced_edge_compatibility_shard3_consistent) {
    surface.failure_reason =
        "final readiness gate advanced edge compatibility workpack shard3 is inconsistent";
  } else if (!surface.advanced_edge_compatibility_shard3_consistent) {
    surface.failure_reason =
        "final readiness gate advanced edge compatibility workpack shard3 consistency is not satisfied";
  } else if (!surface.advanced_edge_compatibility_shard3_ready) {
    surface.failure_reason =
        "final readiness gate advanced edge compatibility workpack shard3 is not ready";
  } else if (surface.advanced_edge_compatibility_shard3_key.empty()) {
    surface.failure_reason =
        "final readiness gate advanced edge compatibility workpack shard3 key is not ready";
  } else if (!inputs.lane_advanced_diagnostics_shard3_consistent) {
    surface.failure_reason =
        "final readiness gate advanced diagnostics workpack shard3 is inconsistent";
  } else if (!surface.advanced_diagnostics_shard3_consistent) {
    surface.failure_reason =
        "final readiness gate advanced diagnostics workpack shard3 consistency is not satisfied";
  } else if (!surface.advanced_diagnostics_shard3_ready) {
    surface.failure_reason =
        "final readiness gate advanced diagnostics workpack shard3 is not ready";
  } else if (surface.advanced_diagnostics_shard3_key.empty()) {
    surface.failure_reason =
        "final readiness gate advanced diagnostics workpack shard3 key is not ready";
  } else if (!inputs.lane_advanced_conformance_shard3_consistent) {
    surface.failure_reason =
        "final readiness gate advanced conformance workpack shard3 is inconsistent";
  } else if (!surface.advanced_conformance_shard3_consistent) {
    surface.failure_reason =
        "final readiness gate advanced conformance workpack shard3 consistency is not satisfied";
  } else if (!surface.advanced_conformance_shard3_ready) {
    surface.failure_reason =
        "final readiness gate advanced conformance workpack shard3 is not ready";
  } else if (surface.advanced_conformance_shard3_key.empty()) {
    surface.failure_reason =
        "final readiness gate advanced conformance workpack shard3 key is not ready";
  } else if (!inputs.lane_advanced_integration_shard3_consistent) {
    surface.failure_reason =
        "final readiness gate advanced integration workpack shard3 is inconsistent";
  } else if (!surface.advanced_integration_shard3_consistent) {
    surface.failure_reason =
        "final readiness gate advanced integration workpack shard3 consistency is not satisfied";
  } else if (!surface.advanced_integration_shard3_ready) {
    surface.failure_reason =
        "final readiness gate advanced integration workpack shard3 is not ready";
  } else if (surface.advanced_integration_shard3_key.empty()) {
    surface.failure_reason =
        "final readiness gate advanced integration workpack shard3 key is not ready";
  } else if (!inputs.lane_advanced_performance_shard3_consistent) {
    surface.failure_reason =
        "final readiness gate advanced performance workpack shard3 is inconsistent";
  } else if (!surface.advanced_performance_shard3_consistent) {
    surface.failure_reason =
        "final readiness gate advanced performance workpack shard3 consistency is not satisfied";
  } else if (!surface.advanced_performance_shard3_ready) {
    surface.failure_reason =
        "final readiness gate advanced performance workpack shard3 is not ready";
  } else if (surface.advanced_performance_shard3_key.empty()) {
    surface.failure_reason =
        "final readiness gate advanced performance workpack shard3 key is not ready";
  } else if (!inputs.lane_advanced_core_shard4_consistent) {
    surface.failure_reason =
        "final readiness gate advanced core workpack shard4 is inconsistent";
  } else if (!surface.advanced_core_shard4_consistent) {
    surface.failure_reason =
        "final readiness gate advanced core workpack shard4 consistency is not satisfied";
  } else if (!surface.advanced_core_shard4_ready) {
    surface.failure_reason =
        "final readiness gate advanced core workpack shard4 is not ready";
  } else if (surface.advanced_core_shard4_key.empty()) {
    surface.failure_reason =
        "final readiness gate advanced core workpack shard4 key is not ready";
  } else if (!inputs.lane_advanced_edge_compatibility_shard4_consistent) {
    surface.failure_reason =
        "final readiness gate advanced edge compatibility workpack shard4 is inconsistent";
  } else if (!surface.advanced_edge_compatibility_shard4_consistent) {
    surface.failure_reason =
        "final readiness gate advanced edge compatibility workpack shard4 consistency is not satisfied";
  } else if (!surface.advanced_edge_compatibility_shard4_ready) {
    surface.failure_reason =
        "final readiness gate advanced edge compatibility workpack shard4 is not ready";
  } else if (surface.advanced_edge_compatibility_shard4_key.empty()) {
    surface.failure_reason =
        "final readiness gate advanced edge compatibility workpack shard4 key is not ready";
  } else if (!inputs.lane_advanced_integration_closeout_signoff_consistent) {
    surface.failure_reason =
        "final readiness gate integration closeout and gate sign-off is inconsistent";
  } else if (!surface.advanced_integration_closeout_signoff_consistent) {
    surface.failure_reason =
        "final readiness gate integration closeout and gate sign-off consistency is not satisfied";
  } else if (!surface.advanced_integration_closeout_signoff_ready) {
    surface.failure_reason =
        "final readiness gate integration closeout and gate sign-off is not ready";
  } else if (surface.advanced_integration_closeout_signoff_key.empty()) {
    surface.failure_reason =
        "final readiness gate integration closeout and gate sign-off key is not ready";
  } else if (!inputs.lane_advanced_edge_compatibility_shard2_consistent) {
    surface.failure_reason =
        "final readiness gate advanced edge compatibility workpack shard2 is inconsistent";
  } else if (!surface.advanced_edge_compatibility_shard2_consistent) {
    surface.failure_reason =
        "final readiness gate advanced edge compatibility workpack shard2 consistency is not satisfied";
  } else if (!surface.advanced_edge_compatibility_shard2_ready) {
    surface.failure_reason =
        "final readiness gate advanced edge compatibility workpack shard2 is not ready";
  } else if (surface.advanced_edge_compatibility_shard2_key.empty()) {
    surface.failure_reason =
        "final readiness gate advanced edge compatibility workpack shard2 key is not ready";
  } else if (!inputs.lane_advanced_diagnostics_shard2_consistent) {
    surface.failure_reason =
        "final readiness gate advanced diagnostics workpack shard2 is inconsistent";
  } else if (!surface.advanced_diagnostics_shard2_consistent) {
    surface.failure_reason =
        "final readiness gate advanced diagnostics workpack shard2 consistency is not satisfied";
  } else if (!surface.advanced_diagnostics_shard2_ready) {
    surface.failure_reason =
        "final readiness gate advanced diagnostics workpack shard2 is not ready";
  } else if (surface.advanced_diagnostics_shard2_key.empty()) {
    surface.failure_reason =
        "final readiness gate advanced diagnostics workpack shard2 key is not ready";
  } else if (!inputs.lane_advanced_conformance_shard2_consistent) {
    surface.failure_reason =
        "final readiness gate advanced conformance workpack shard2 is inconsistent";
  } else if (!surface.advanced_conformance_shard2_consistent) {
    surface.failure_reason =
        "final readiness gate advanced conformance workpack shard2 consistency is not satisfied";
  } else if (!surface.advanced_conformance_shard2_ready) {
    surface.failure_reason =
        "final readiness gate advanced conformance workpack shard2 is not ready";
  } else if (surface.advanced_conformance_shard2_key.empty()) {
    surface.failure_reason =
        "final readiness gate advanced conformance workpack shard2 key is not ready";
  } else if (!inputs.lane_advanced_integration_shard2_consistent) {
    surface.failure_reason =
        "final readiness gate advanced integration workpack shard2 is inconsistent";
  } else if (!surface.advanced_integration_shard2_consistent) {
    surface.failure_reason =
        "final readiness gate advanced integration workpack shard2 consistency is not satisfied";
  } else if (!surface.advanced_integration_shard2_ready) {
    surface.failure_reason =
        "final readiness gate advanced integration workpack shard2 is not ready";
  } else if (surface.advanced_integration_shard2_key.empty()) {
    surface.failure_reason =
        "final readiness gate advanced integration workpack shard2 key is not ready";
  } else if (!inputs.lane_advanced_performance_shard2_consistent) {
    surface.failure_reason =
        "final readiness gate advanced performance workpack shard2 is inconsistent";
  } else if (!surface.advanced_performance_shard2_consistent) {
    surface.failure_reason =
        "final readiness gate advanced performance workpack shard2 consistency is not satisfied";
  } else if (!surface.advanced_performance_shard2_ready) {
    surface.failure_reason =
        "final readiness gate advanced performance workpack shard2 is not ready";
  } else if (surface.advanced_performance_shard2_key.empty()) {
    surface.failure_reason =
        "final readiness gate advanced performance workpack shard2 key is not ready";
  } else if (!inputs.lane_integration_closeout_signoff_consistent) {
    surface.failure_reason =
        "final readiness gate integration closeout and gate sign-off is inconsistent";
  } else if (!surface.integration_closeout_signoff_consistent) {
    surface.failure_reason =
        "final readiness gate integration closeout and gate sign-off consistency is not satisfied";
  } else if (!surface.integration_closeout_signoff_ready) {
    surface.failure_reason =
        "final readiness gate integration closeout and gate sign-off is not ready";
  } else if (surface.integration_closeout_signoff_key.empty()) {
    surface.failure_reason =
        "final readiness gate integration closeout and gate sign-off key is not ready";
  } else {
    surface.failure_reason =
        "final readiness gate core feature implementation is not ready";
  }
}

bool IsObjc3FinalReadinessGateCoreFeatureImplementationSurfaceReady(
    const Objc3FinalReadinessGateCoreFeatureImplementationSurface &surface,
    std::string &reason) {
  if (surface.core_feature_impl_ready) {
    reason.clear();
    return true;
  }

  reason = surface.failure_reason.empty()
               ? "final readiness gate core feature implementation is not ready"
               : surface.failure_reason;
  return false;
}
