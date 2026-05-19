#include "pipeline/readiness/objc3_final_readiness_gate_failure_reasons_private.h"

namespace objc3_final_readiness_gate_failure_reason_detail {

const char *FindObjc3FinalReadinessGateFoundationFailureReason(
    const Objc3FinalReadinessGateCoreFeatureImplementationSurface &surface,
    const Objc3FinalReadinessGateFailureReasonInputs &inputs) {
  if (!surface.governance_contract_ready) {
    return "final readiness gate governance contract is not ready";
  }

  if (!surface.modular_split_ready) {
    return "final readiness gate modular split scaffold is not ready";
  }

  if (!surface.lane_a_core_feature_ready) {
    return "final readiness gate lane-A core feature readiness is not satisfied";
  }

  if (!surface.lane_b_core_feature_ready) {
    return "final readiness gate lane-B core feature readiness is not satisfied";
  }

  if (!surface.lane_c_core_feature_ready) {
    return "final readiness gate lane-C core feature readiness is not satisfied";
  }

  if (!surface.lane_d_core_feature_ready) {
    return "final readiness gate lane-D core feature readiness is not satisfied";
  }

  if (!inputs.replay_keys_ready) {
    return "final readiness gate dependency replay keys are not ready";
  }

  if (!inputs.lane_expansion_consistent) {
    return "final readiness gate core feature expansion is inconsistent";
  }

  if (!surface.core_feature_expansion_consistent) {
    return "final readiness gate core feature expansion consistency is not satisfied";
  }

  if (!surface.core_feature_expansion_ready) {
    return "final readiness gate core feature expansion is not ready";
  }

  if (surface.core_feature_expansion_key.empty()) {
    return "final readiness gate core feature expansion key is not ready";
  }

  if (!inputs.lane_edge_case_compatibility_consistent) {
    return "final readiness gate edge-case compatibility is inconsistent";
  }

  if (!surface.edge_case_compatibility_consistent) {
    return "final readiness gate edge-case compatibility consistency is not satisfied";
  }

  if (!surface.edge_case_compatibility_ready) {
    return "final readiness gate edge-case compatibility is not ready";
  }

  if (surface.edge_case_compatibility_key.empty()) {
    return "final readiness gate edge-case compatibility key is not ready";
  }

  if (!inputs.lane_edge_case_expansion_consistent) {
    return "final readiness gate edge-case expansion is inconsistent";
  }

  if (!surface.edge_case_expansion_consistent) {
    return "final readiness gate edge-case expansion consistency is not satisfied";
  }

  if (!surface.edge_case_robustness_ready) {
    return "final readiness gate edge-case robustness is not ready";
  }

  if (surface.edge_case_robustness_key.empty()) {
    return "final readiness gate edge-case robustness key is not ready";
  }

  if (!inputs.lane_diagnostics_hardening_consistent) {
    return "final readiness gate diagnostics hardening is inconsistent";
  }

  if (!surface.diagnostics_hardening_consistent) {
    return "final readiness gate diagnostics hardening consistency is not satisfied";
  }

  if (!surface.diagnostics_hardening_ready) {
    return "final readiness gate diagnostics hardening is not ready";
  }

  if (surface.diagnostics_hardening_key.empty()) {
    return "final readiness gate diagnostics hardening key is not ready";
  }

  if (!inputs.lane_recovery_determinism_consistent) {
    return "final readiness gate recovery and determinism hardening is inconsistent";
  }

  if (!surface.recovery_determinism_consistent) {
    return "final readiness gate recovery and determinism consistency is not satisfied";
  }

  if (!surface.recovery_determinism_ready) {
    return "final readiness gate recovery and determinism hardening is not ready";
  }

  if (surface.recovery_determinism_key.empty()) {
    return "final readiness gate recovery and determinism key is not ready";
  }

  if (!inputs.lane_conformance_matrix_consistent) {
    return "final readiness gate conformance matrix is inconsistent";
  }

  if (!surface.conformance_matrix_consistent) {
    return "final readiness gate conformance matrix consistency is not satisfied";
  }

  if (!surface.conformance_matrix_ready) {
    return "final readiness gate conformance matrix is not ready";
  }

  if (surface.conformance_matrix_key.empty()) {
    return "final readiness gate conformance matrix key is not ready";
  }

  if (!inputs.lane_conformance_corpus_consistent) {
    return "final readiness gate conformance corpus is inconsistent";
  }

  if (!surface.conformance_corpus_consistent) {
    return "final readiness gate conformance corpus consistency is not satisfied";
  }

  if (!surface.conformance_corpus_ready) {
    return "final readiness gate conformance corpus is not ready";
  }

  if (surface.conformance_corpus_key.empty()) {
    return "final readiness gate conformance corpus key is not ready";
  }

  if (!inputs.lane_performance_quality_guardrails_consistent) {
    return "final readiness gate performance and quality guardrails are inconsistent";
  }

  if (!surface.performance_quality_guardrails_consistent) {
    return "final readiness gate performance and quality guardrails consistency is not satisfied";
  }

  if (!surface.performance_quality_guardrails_ready) {
    return "final readiness gate performance and quality guardrails are not ready";
  }

  if (surface.performance_quality_guardrails_key.empty()) {
    return "final readiness gate performance and quality guardrails key is not ready";
  }

  if (!inputs.lane_cross_lane_integration_consistent) {
    return "final readiness gate cross-lane integration sync is inconsistent";
  }

  if (!surface.cross_lane_integration_consistent) {
    return "final readiness gate cross-lane integration sync consistency is not satisfied";
  }

  if (!surface.cross_lane_integration_ready) {
    return "final readiness gate cross-lane integration sync is not ready";
  }

  if (surface.cross_lane_integration_key.empty()) {
    return "final readiness gate cross-lane integration sync key is not ready";
  }

  if (!inputs.lane_docs_runbook_sync_consistent) {
    return "final readiness gate docs and operator runbook sync is inconsistent";
  }

  if (!surface.docs_runbook_sync_consistent) {
    return "final readiness gate docs and operator runbook sync consistency is not satisfied";
  }

  if (!surface.docs_runbook_sync_ready) {
    return "final readiness gate docs and operator runbook sync is not ready";
  }

  if (surface.docs_runbook_sync_key.empty()) {
    return "final readiness gate docs and operator runbook sync key is not ready";
  }

  if (!inputs.lane_release_candidate_replay_dry_run_consistent) {
    return "final readiness gate release candidate replay dry-run is inconsistent";
  }

  if (!surface.release_candidate_replay_dry_run_consistent) {
    return "final readiness gate release candidate replay dry-run consistency is not satisfied";
  }

  if (!surface.release_candidate_replay_dry_run_ready) {
    return "final readiness gate release candidate replay dry-run is not ready";
  }

  if (surface.release_candidate_replay_dry_run_key.empty()) {
    return "final readiness gate release candidate replay dry-run key is not ready";
  }

  return nullptr;
}

}  // namespace objc3_final_readiness_gate_failure_reason_detail
