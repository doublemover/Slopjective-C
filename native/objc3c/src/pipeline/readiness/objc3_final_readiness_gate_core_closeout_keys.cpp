#include <sstream>
#include <string>

#include "pipeline/readiness/objc3_final_readiness_gate_core_keys.h"

std::string BuildObjc3FinalReadinessGatePerformanceQualityGuardrailsKey(
    const Objc3FinalReadinessGateCoreFeatureImplementationSurface &surface,
    bool lane_a_core_feature_expansion_ready,
    bool lane_b_edge_case_compatibility_ready,
    bool lane_c_edge_case_compatibility_ready,
    bool lane_d_conformance_matrix_ready,
    bool lane_d_conformance_matrix_key_ready) {
  std::ostringstream key;
  key << "final-readiness-gate-performance-quality-guardrails:v1:"
      << "dependency-chain-ready="
      << (surface.dependency_chain_ready ? "true" : "false")
      << ";conformance-corpus-ready="
      << (surface.conformance_corpus_ready ? "true" : "false")
      << ";lane-a-core-feature-expansion-ready="
      << (lane_a_core_feature_expansion_ready ? "true" : "false")
      << ";lane-b-edge-case-compatibility-ready="
      << (lane_b_edge_case_compatibility_ready ? "true" : "false")
      << ";lane-c-edge-case-compatibility-ready="
      << (lane_c_edge_case_compatibility_ready ? "true" : "false")
      << ";lane-d-conformance-matrix-ready="
      << (lane_d_conformance_matrix_ready ? "true" : "false")
      << ";lane-d-conformance-matrix-key-ready="
      << (lane_d_conformance_matrix_key_ready ? "true" : "false")
      << ";performance-quality-guardrails-consistent="
      << (surface.performance_quality_guardrails_consistent ? "true" : "false")
      << ";performance-quality-guardrails-ready="
      << (surface.performance_quality_guardrails_ready ? "true" : "false");
  return key.str();
}

std::string BuildObjc3FinalReadinessGateCrossLaneIntegrationKey(
    const Objc3FinalReadinessGateCoreFeatureImplementationSurface &surface,
    bool lane_a_core_feature_expansion_ready,
    bool lane_b_edge_case_compatibility_ready,
    bool lane_c_edge_case_robustness_ready,
    bool lane_d_conformance_matrix_ready,
    bool lane_d_conformance_matrix_key_ready) {
  std::ostringstream key;
  key << "final-readiness-gate-cross-lane-integration:v1:"
      << "dependency-chain-ready="
      << (surface.dependency_chain_ready ? "true" : "false")
      << ";performance-quality-guardrails-ready="
      << (surface.performance_quality_guardrails_ready ? "true" : "false")
      << ";lane-a-core-feature-expansion-ready="
      << (lane_a_core_feature_expansion_ready ? "true" : "false")
      << ";lane-b-edge-case-compatibility-ready="
      << (lane_b_edge_case_compatibility_ready ? "true" : "false")
      << ";lane-c-edge-case-robustness-ready="
      << (lane_c_edge_case_robustness_ready ? "true" : "false")
      << ";lane-d-conformance-matrix-ready="
      << (lane_d_conformance_matrix_ready ? "true" : "false")
      << ";lane-d-conformance-matrix-key-ready="
      << (lane_d_conformance_matrix_key_ready ? "true" : "false")
      << ";cross-lane-integration-consistent="
      << (surface.cross_lane_integration_consistent ? "true" : "false")
      << ";cross-lane-integration-ready="
      << (surface.cross_lane_integration_ready ? "true" : "false");
  return key.str();
}

std::string BuildObjc3FinalReadinessGateDocsRunbookSyncKey(
    const Objc3FinalReadinessGateCoreFeatureImplementationSurface &surface,
    bool lane_a_edge_case_compatibility_ready,
    bool lane_b_edge_case_robustness_ready,
    bool lane_c_edge_case_robustness_ready,
    bool lane_d_conformance_matrix_ready,
    bool lane_d_conformance_matrix_key_ready) {
  std::ostringstream key;
  key << "final-readiness-gate-docs-runbook-sync:v1:"
      << "dependency-chain-ready="
      << (surface.dependency_chain_ready ? "true" : "false")
      << ";cross-lane-integration-ready="
      << (surface.cross_lane_integration_ready ? "true" : "false")
      << ";lane-a-edge-case-compatibility-ready="
      << (lane_a_edge_case_compatibility_ready ? "true" : "false")
      << ";lane-b-edge-case-robustness-ready="
      << (lane_b_edge_case_robustness_ready ? "true" : "false")
      << ";lane-c-edge-case-robustness-ready="
      << (lane_c_edge_case_robustness_ready ? "true" : "false")
      << ";lane-d-conformance-matrix-ready="
      << (lane_d_conformance_matrix_ready ? "true" : "false")
      << ";lane-d-conformance-matrix-key-ready="
      << (lane_d_conformance_matrix_key_ready ? "true" : "false")
      << ";docs-runbook-sync-consistent="
      << (surface.docs_runbook_sync_consistent ? "true" : "false")
      << ";docs-runbook-sync-ready="
      << (surface.docs_runbook_sync_ready ? "true" : "false");
  return key.str();
}

std::string BuildObjc3FinalReadinessGateReleaseCandidateReplayDryRunKey(
    const Objc3FinalReadinessGateCoreFeatureImplementationSurface &surface,
    bool lane_a_edge_case_compatibility_ready,
    bool lane_b_edge_case_robustness_ready,
    bool lane_c_diagnostics_hardening_ready,
    bool lane_d_core_feature_impl_ready,
    bool lane_d_core_feature_key_ready) {
  std::ostringstream key;
  key << "final-readiness-gate-release-candidate-replay-dry-run:v1:"
      << "dependency-chain-ready="
      << (surface.dependency_chain_ready ? "true" : "false")
      << ";docs-runbook-sync-ready="
      << (surface.docs_runbook_sync_ready ? "true" : "false")
      << ";lane-a-edge-case-compatibility-ready="
      << (lane_a_edge_case_compatibility_ready ? "true" : "false")
      << ";lane-b-edge-case-robustness-ready="
      << (lane_b_edge_case_robustness_ready ? "true" : "false")
      << ";lane-c-diagnostics-hardening-ready="
      << (lane_c_diagnostics_hardening_ready ? "true" : "false")
      << ";lane-d-core-feature-ready="
      << (lane_d_core_feature_impl_ready ? "true" : "false")
      << ";lane-d-core-feature-key-ready="
      << (lane_d_core_feature_key_ready ? "true" : "false")
      << ";release-candidate-replay-dry-run-consistent="
      << (surface.release_candidate_replay_dry_run_consistent ? "true" : "false")
      << ";release-candidate-replay-dry-run-ready="
      << (surface.release_candidate_replay_dry_run_ready ? "true" : "false");
  return key.str();
}
