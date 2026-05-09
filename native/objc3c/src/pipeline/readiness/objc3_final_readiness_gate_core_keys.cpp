#include <sstream>
#include <string>

#include "pipeline/readiness/objc3_final_readiness_gate_core_keys.h"

std::string BuildObjc3FinalReadinessGateCoreFeatureImplementationKey(
    const Objc3FinalReadinessGateCoreFeatureImplementationSurface &surface) {
  std::ostringstream key;
  key << "final-readiness-gate-core-feature-implementation:v1:"
      << "governance_contract_ready="
      << (surface.governance_contract_ready ? "true" : "false")
      << ";modular_split_ready="
      << (surface.modular_split_ready ? "true" : "false")
      << ";lane_a_core_feature_ready="
      << (surface.lane_a_core_feature_ready ? "true" : "false")
      << ";lane_b_core_feature_ready="
      << (surface.lane_b_core_feature_ready ? "true" : "false")
      << ";lane_c_core_feature_ready="
      << (surface.lane_c_core_feature_ready ? "true" : "false")
      << ";lane_d_core_feature_ready="
      << (surface.lane_d_core_feature_ready ? "true" : "false")
      << ";dependency_chain_ready="
      << (surface.dependency_chain_ready ? "true" : "false")
      << ";governance_key_ready=" << (!surface.governance_key.empty() ? "true" : "false")
      << ";modular_split_key_ready="
      << (!surface.modular_split_key.empty() ? "true" : "false")
      << ";lane_a_key_ready=" << (!surface.lane_a_key.empty() ? "true" : "false")
      << ";lane_b_key_ready=" << (!surface.lane_b_key.empty() ? "true" : "false")
      << ";lane_c_key_ready=" << (!surface.lane_c_key.empty() ? "true" : "false")
      << ";lane_d_key_ready=" << (!surface.lane_d_key.empty() ? "true" : "false")
      << ";core_feature_expansion_consistent="
      << (surface.core_feature_expansion_consistent ? "true" : "false")
      << ";core_feature_expansion_ready="
      << (surface.core_feature_expansion_ready ? "true" : "false")
      << ";core_feature_expansion_key_ready="
      << (!surface.core_feature_expansion_key.empty() ? "true" : "false")
      << ";edge_case_compatibility_consistent="
      << (surface.edge_case_compatibility_consistent ? "true" : "false")
      << ";edge_case_compatibility_ready="
      << (surface.edge_case_compatibility_ready ? "true" : "false")
      << ";edge_case_compatibility_key_ready="
      << (!surface.edge_case_compatibility_key.empty() ? "true" : "false")
      << ";edge_case_expansion_consistent="
      << (surface.edge_case_expansion_consistent ? "true" : "false")
      << ";edge_case_robustness_ready="
      << (surface.edge_case_robustness_ready ? "true" : "false")
      << ";edge_case_robustness_key_ready="
      << (!surface.edge_case_robustness_key.empty() ? "true" : "false")
      << ";diagnostics_hardening_consistent="
      << (surface.diagnostics_hardening_consistent ? "true" : "false")
      << ";diagnostics_hardening_ready="
      << (surface.diagnostics_hardening_ready ? "true" : "false")
      << ";diagnostics_hardening_key_ready="
      << (!surface.diagnostics_hardening_key.empty() ? "true" : "false")
      << ";recovery_determinism_consistent="
      << (surface.recovery_determinism_consistent ? "true" : "false")
      << ";recovery_determinism_ready="
      << (surface.recovery_determinism_ready ? "true" : "false")
      << ";recovery_determinism_key_ready="
      << (!surface.recovery_determinism_key.empty() ? "true" : "false")
      << ";conformance_matrix_consistent="
      << (surface.conformance_matrix_consistent ? "true" : "false")
      << ";conformance_matrix_ready="
      << (surface.conformance_matrix_ready ? "true" : "false")
      << ";conformance_matrix_key_ready="
      << (!surface.conformance_matrix_key.empty() ? "true" : "false")
      << ";conformance_corpus_consistent="
      << (surface.conformance_corpus_consistent ? "true" : "false")
      << ";conformance_corpus_ready="
      << (surface.conformance_corpus_ready ? "true" : "false")
      << ";conformance_corpus_key_ready="
      << (!surface.conformance_corpus_key.empty() ? "true" : "false")
      << ";performance_quality_guardrails_consistent="
      << (surface.performance_quality_guardrails_consistent ? "true" : "false")
      << ";performance_quality_guardrails_ready="
      << (surface.performance_quality_guardrails_ready ? "true" : "false")
      << ";performance_quality_guardrails_key_ready="
      << (!surface.performance_quality_guardrails_key.empty() ? "true" : "false")
      << ";cross_lane_integration_consistent="
      << (surface.cross_lane_integration_consistent ? "true" : "false")
      << ";cross_lane_integration_ready="
      << (surface.cross_lane_integration_ready ? "true" : "false")
      << ";cross_lane_integration_key_ready="
      << (!surface.cross_lane_integration_key.empty() ? "true" : "false")
      << ";docs_runbook_sync_consistent="
      << (surface.docs_runbook_sync_consistent ? "true" : "false")
      << ";docs_runbook_sync_ready="
      << (surface.docs_runbook_sync_ready ? "true" : "false")
      << ";docs_runbook_sync_key_ready="
      << (!surface.docs_runbook_sync_key.empty() ? "true" : "false")
      << ";release_candidate_replay_dry_run_consistent="
      << (surface.release_candidate_replay_dry_run_consistent ? "true" : "false")
      << ";release_candidate_replay_dry_run_ready="
      << (surface.release_candidate_replay_dry_run_ready ? "true" : "false")
      << ";release_candidate_replay_dry_run_key_ready="
      << (!surface.release_candidate_replay_dry_run_key.empty() ? "true" : "false")
      << ";advanced_core_shard1_consistent="
      << (surface.advanced_core_shard1_consistent ? "true" : "false")
      << ";advanced_core_shard1_ready="
      << (surface.advanced_core_shard1_ready ? "true" : "false")
      << ";advanced_core_shard1_key_ready="
      << (!surface.advanced_core_shard1_key.empty() ? "true" : "false")
      << ";advanced_edge_compatibility_shard1_consistent="
      << (surface.advanced_edge_compatibility_shard1_consistent ? "true" : "false")
      << ";advanced_edge_compatibility_shard1_ready="
      << (surface.advanced_edge_compatibility_shard1_ready ? "true" : "false")
      << ";advanced_edge_compatibility_shard1_key_ready="
      << (!surface.advanced_edge_compatibility_shard1_key.empty() ? "true" : "false")
      << ";advanced_diagnostics_shard1_consistent="
      << (surface.advanced_diagnostics_shard1_consistent ? "true" : "false")
      << ";advanced_diagnostics_shard1_ready="
      << (surface.advanced_diagnostics_shard1_ready ? "true" : "false")
      << ";advanced_diagnostics_shard1_key_ready="
      << (!surface.advanced_diagnostics_shard1_key.empty() ? "true" : "false")
      << ";advanced_conformance_shard1_consistent="
      << (surface.advanced_conformance_shard1_consistent ? "true" : "false")
      << ";advanced_conformance_shard1_ready="
      << (surface.advanced_conformance_shard1_ready ? "true" : "false")
      << ";advanced_conformance_shard1_key_ready="
      << (!surface.advanced_conformance_shard1_key.empty() ? "true" : "false")
      << ";advanced_integration_shard1_consistent="
      << (surface.advanced_integration_shard1_consistent ? "true" : "false")
      << ";advanced_integration_shard1_ready="
      << (surface.advanced_integration_shard1_ready ? "true" : "false")
      << ";advanced_integration_shard1_key_ready="
      << (!surface.advanced_integration_shard1_key.empty() ? "true" : "false")
      << ";advanced_performance_shard1_consistent="
      << (surface.advanced_performance_shard1_consistent ? "true" : "false")
      << ";advanced_performance_shard1_ready="
      << (surface.advanced_performance_shard1_ready ? "true" : "false")
      << ";advanced_performance_shard1_key_ready="
      << (!surface.advanced_performance_shard1_key.empty() ? "true" : "false")
      << ";advanced_core_shard2_consistent="
      << (surface.advanced_core_shard2_consistent ? "true" : "false")
      << ";advanced_core_shard2_ready="
      << (surface.advanced_core_shard2_ready ? "true" : "false")
      << ";advanced_core_shard2_key_ready="
      << (!surface.advanced_core_shard2_key.empty() ? "true" : "false")
      << ";advanced_core_shard3_consistent="
      << (surface.advanced_core_shard3_consistent ? "true" : "false")
      << ";advanced_core_shard3_ready="
      << (surface.advanced_core_shard3_ready ? "true" : "false")
      << ";advanced_core_shard3_key_ready="
      << (!surface.advanced_core_shard3_key.empty() ? "true" : "false")
      << ";advanced_edge_compatibility_shard2_consistent="
      << (surface.advanced_edge_compatibility_shard2_consistent ? "true" : "false")
      << ";advanced_edge_compatibility_shard2_ready="
      << (surface.advanced_edge_compatibility_shard2_ready ? "true" : "false")
      << ";advanced_edge_compatibility_shard2_key_ready="
      << (!surface.advanced_edge_compatibility_shard2_key.empty() ? "true" : "false")
      << ";advanced_edge_compatibility_shard3_consistent="
      << (surface.advanced_edge_compatibility_shard3_consistent ? "true" : "false")
      << ";advanced_edge_compatibility_shard3_ready="
      << (surface.advanced_edge_compatibility_shard3_ready ? "true" : "false")
      << ";advanced_edge_compatibility_shard3_key_ready="
      << (!surface.advanced_edge_compatibility_shard3_key.empty() ? "true" : "false")
      << ";advanced_diagnostics_shard2_consistent="
      << (surface.advanced_diagnostics_shard2_consistent ? "true" : "false")
      << ";advanced_diagnostics_shard2_ready="
      << (surface.advanced_diagnostics_shard2_ready ? "true" : "false")
      << ";advanced_diagnostics_shard2_key_ready="
      << (!surface.advanced_diagnostics_shard2_key.empty() ? "true" : "false")
      << ";advanced_diagnostics_shard3_consistent="
      << (surface.advanced_diagnostics_shard3_consistent ? "true" : "false")
      << ";advanced_diagnostics_shard3_ready="
      << (surface.advanced_diagnostics_shard3_ready ? "true" : "false")
      << ";advanced_diagnostics_shard3_key_ready="
      << (!surface.advanced_diagnostics_shard3_key.empty() ? "true" : "false")
      << ";advanced_conformance_shard3_consistent="
      << (surface.advanced_conformance_shard3_consistent ? "true" : "false")
      << ";advanced_conformance_shard3_ready="
      << (surface.advanced_conformance_shard3_ready ? "true" : "false")
      << ";advanced_conformance_shard3_key_ready="
      << (!surface.advanced_conformance_shard3_key.empty() ? "true" : "false")
      << ";advanced_integration_shard3_consistent="
      << (surface.advanced_integration_shard3_consistent ? "true" : "false")
      << ";advanced_integration_shard3_ready="
      << (surface.advanced_integration_shard3_ready ? "true" : "false")
      << ";advanced_integration_shard3_key_ready="
      << (!surface.advanced_integration_shard3_key.empty() ? "true" : "false")
      << ";advanced_performance_shard3_consistent="
      << (surface.advanced_performance_shard3_consistent ? "true" : "false")
      << ";advanced_performance_shard3_ready="
      << (surface.advanced_performance_shard3_ready ? "true" : "false")
      << ";advanced_performance_shard3_key_ready="
      << (!surface.advanced_performance_shard3_key.empty() ? "true" : "false")
      << ";advanced_core_shard4_consistent="
      << (surface.advanced_core_shard4_consistent ? "true" : "false")
      << ";advanced_core_shard4_ready="
      << (surface.advanced_core_shard4_ready ? "true" : "false")
      << ";advanced_core_shard4_key_ready="
      << (!surface.advanced_core_shard4_key.empty() ? "true" : "false")
      << ";advanced_edge_compatibility_shard4_consistent="
      << (surface.advanced_edge_compatibility_shard4_consistent ? "true" : "false")
      << ";advanced_edge_compatibility_shard4_ready="
      << (surface.advanced_edge_compatibility_shard4_ready ? "true" : "false")
      << ";advanced_edge_compatibility_shard4_key_ready="
      << (!surface.advanced_edge_compatibility_shard4_key.empty() ? "true" : "false")
      << ";advanced_integration_closeout_signoff_consistent="
      << (surface.advanced_integration_closeout_signoff_consistent ? "true" : "false")
      << ";advanced_integration_closeout_signoff_ready="
      << (surface.advanced_integration_closeout_signoff_ready ? "true" : "false")
      << ";advanced_integration_closeout_signoff_key_ready="
      << (!surface.advanced_integration_closeout_signoff_key.empty() ? "true" : "false")
      << ";advanced_conformance_shard2_consistent="
      << (surface.advanced_conformance_shard2_consistent ? "true" : "false")
      << ";advanced_conformance_shard2_ready="
      << (surface.advanced_conformance_shard2_ready ? "true" : "false")
      << ";advanced_conformance_shard2_key_ready="
      << (!surface.advanced_conformance_shard2_key.empty() ? "true" : "false")
      << ";advanced_integration_shard2_consistent="
      << (surface.advanced_integration_shard2_consistent ? "true" : "false")
      << ";advanced_integration_shard2_ready="
      << (surface.advanced_integration_shard2_ready ? "true" : "false")
      << ";advanced_integration_shard2_key_ready="
      << (!surface.advanced_integration_shard2_key.empty() ? "true" : "false")
      << ";advanced_performance_shard2_consistent="
      << (surface.advanced_performance_shard2_consistent ? "true" : "false")
      << ";advanced_performance_shard2_ready="
      << (surface.advanced_performance_shard2_ready ? "true" : "false")
      << ";advanced_performance_shard2_key_ready="
      << (!surface.advanced_performance_shard2_key.empty() ? "true" : "false")
      << ";integration_closeout_signoff_consistent="
      << (surface.integration_closeout_signoff_consistent ? "true" : "false")
      << ";integration_closeout_signoff_ready="
      << (surface.integration_closeout_signoff_ready ? "true" : "false")
      << ";integration_closeout_signoff_key_ready="
      << (!surface.integration_closeout_signoff_key.empty() ? "true" : "false")
      << ";core_feature_impl_ready="
      << (surface.core_feature_impl_ready ? "true" : "false");
  return key.str();
}

std::string BuildObjc3FinalReadinessGateEdgeCaseCompatibilityKey(
    const Objc3FinalReadinessGateCoreFeatureImplementationSurface &surface,
    bool lane_a_edge_case_compatibility_ready,
    bool lane_b_edge_case_compatibility_ready,
    bool lane_c_edge_case_compatibility_ready,
    bool lane_d_edge_case_compatibility_ready) {
  std::ostringstream key;
  key << "final-readiness-gate-edge-case-compatibility:v1:"
      << "dependency-chain-ready="
      << (surface.dependency_chain_ready ? "true" : "false")
      << ";core-feature-expansion-ready="
      << (surface.core_feature_expansion_ready ? "true" : "false")
      << ";lane-a-edge-case-compatibility-ready="
      << (lane_a_edge_case_compatibility_ready ? "true" : "false")
      << ";lane-b-edge-case-compatibility-ready="
      << (lane_b_edge_case_compatibility_ready ? "true" : "false")
      << ";lane-c-edge-case-compatibility-ready="
      << (lane_c_edge_case_compatibility_ready ? "true" : "false")
      << ";lane-d-edge-case-compatibility-ready="
      << (lane_d_edge_case_compatibility_ready ? "true" : "false")
      << ";edge-case-compatibility-consistent="
      << (surface.edge_case_compatibility_consistent ? "true" : "false")
      << ";edge-case-compatibility-ready="
      << (surface.edge_case_compatibility_ready ? "true" : "false");
  return key.str();
}

std::string BuildObjc3FinalReadinessGateEdgeCaseRobustnessKey(
    const Objc3FinalReadinessGateCoreFeatureImplementationSurface &surface,
    bool lane_a_core_feature_ready,
    bool lane_b_core_feature_ready,
    bool lane_c_core_feature_ready,
    bool lane_d_edge_case_compatibility_ready) {
  std::ostringstream key;
  key << "final-readiness-gate-edge-case-robustness:v1:"
      << "dependency-chain-ready="
      << (surface.dependency_chain_ready ? "true" : "false")
      << ";edge-case-compatibility-ready="
      << (surface.edge_case_compatibility_ready ? "true" : "false")
      << ";lane-a-core-feature-ready="
      << (lane_a_core_feature_ready ? "true" : "false")
      << ";lane-b-core-feature-ready="
      << (lane_b_core_feature_ready ? "true" : "false")
      << ";lane-c-core-feature-ready="
      << (lane_c_core_feature_ready ? "true" : "false")
      << ";lane-d-edge-case-compatibility-ready="
      << (lane_d_edge_case_compatibility_ready ? "true" : "false")
      << ";edge-case-expansion-consistent="
      << (surface.edge_case_expansion_consistent ? "true" : "false")
      << ";edge-case-robustness-ready="
      << (surface.edge_case_robustness_ready ? "true" : "false");
  return key.str();
}

std::string BuildObjc3FinalReadinessGateDiagnosticsHardeningKey(
    const Objc3FinalReadinessGateCoreFeatureImplementationSurface &surface,
    bool lane_a_core_feature_ready,
    bool lane_b_core_feature_ready,
    bool lane_c_core_feature_ready,
    bool lane_d_edge_case_robustness_ready) {
  std::ostringstream key;
  key << "final-readiness-gate-diagnostics-hardening:v1:"
      << "dependency-chain-ready="
      << (surface.dependency_chain_ready ? "true" : "false")
      << ";edge-case-robustness-ready="
      << (surface.edge_case_robustness_ready ? "true" : "false")
      << ";lane-a-core-feature-ready="
      << (lane_a_core_feature_ready ? "true" : "false")
      << ";lane-b-core-feature-ready="
      << (lane_b_core_feature_ready ? "true" : "false")
      << ";lane-c-core-feature-ready="
      << (lane_c_core_feature_ready ? "true" : "false")
      << ";lane-d-edge-case-robustness-ready="
      << (lane_d_edge_case_robustness_ready ? "true" : "false")
      << ";diagnostics-hardening-consistent="
      << (surface.diagnostics_hardening_consistent ? "true" : "false")
      << ";diagnostics-hardening-ready="
      << (surface.diagnostics_hardening_ready ? "true" : "false");
  return key.str();
}

std::string BuildObjc3FinalReadinessGateRecoveryDeterminismKey(
    const Objc3FinalReadinessGateCoreFeatureImplementationSurface &surface,
    bool lane_a_core_feature_ready,
    bool lane_b_core_feature_expansion_ready,
    bool lane_c_core_feature_expansion_ready,
    bool lane_d_diagnostics_hardening_ready) {
  std::ostringstream key;
  key << "final-readiness-gate-recovery-determinism:v1:"
      << "dependency-chain-ready="
      << (surface.dependency_chain_ready ? "true" : "false")
      << ";diagnostics-hardening-ready="
      << (surface.diagnostics_hardening_ready ? "true" : "false")
      << ";lane-a-core-feature-ready="
      << (lane_a_core_feature_ready ? "true" : "false")
      << ";lane-b-core-feature-expansion-ready="
      << (lane_b_core_feature_expansion_ready ? "true" : "false")
      << ";lane-c-core-feature-expansion-ready="
      << (lane_c_core_feature_expansion_ready ? "true" : "false")
      << ";lane-d-diagnostics-hardening-ready="
      << (lane_d_diagnostics_hardening_ready ? "true" : "false")
      << ";recovery-determinism-consistent="
      << (surface.recovery_determinism_consistent ? "true" : "false")
      << ";recovery-determinism-ready="
      << (surface.recovery_determinism_ready ? "true" : "false");
  return key.str();
}

std::string BuildObjc3FinalReadinessGateConformanceMatrixKey(
    const Objc3FinalReadinessGateCoreFeatureImplementationSurface &surface,
    bool lane_a_core_feature_ready,
    bool lane_b_core_feature_expansion_ready,
    bool lane_c_core_feature_expansion_ready,
    bool lane_d_diagnostics_hardening_ready) {
  std::ostringstream key;
  key << "final-readiness-gate-conformance-matrix:v1:"
      << "dependency-chain-ready="
      << (surface.dependency_chain_ready ? "true" : "false")
      << ";recovery-determinism-ready="
      << (surface.recovery_determinism_ready ? "true" : "false")
      << ";lane-a-core-feature-ready="
      << (lane_a_core_feature_ready ? "true" : "false")
      << ";lane-b-core-feature-expansion-ready="
      << (lane_b_core_feature_expansion_ready ? "true" : "false")
      << ";lane-c-core-feature-expansion-ready="
      << (lane_c_core_feature_expansion_ready ? "true" : "false")
      << ";lane-d-diagnostics-hardening-ready="
      << (lane_d_diagnostics_hardening_ready ? "true" : "false")
      << ";conformance-matrix-consistent="
      << (surface.conformance_matrix_consistent ? "true" : "false")
      << ";conformance-matrix-ready="
      << (surface.conformance_matrix_ready ? "true" : "false");
  return key.str();
}

std::string BuildObjc3FinalReadinessGateConformanceCorpusKey(
    const Objc3FinalReadinessGateCoreFeatureImplementationSurface &surface,
    bool lane_a_core_feature_expansion_ready,
    bool lane_b_core_feature_expansion_ready,
    bool lane_c_edge_case_compatibility_ready,
    bool lane_d_recovery_determinism_ready,
    bool lane_d_recovery_determinism_key_ready) {
  std::ostringstream key;
  key << "final-readiness-gate-conformance-corpus:v1:"
      << "dependency-chain-ready="
      << (surface.dependency_chain_ready ? "true" : "false")
      << ";conformance-matrix-ready="
      << (surface.conformance_matrix_ready ? "true" : "false")
      << ";lane-a-core-feature-expansion-ready="
      << (lane_a_core_feature_expansion_ready ? "true" : "false")
      << ";lane-b-core-feature-expansion-ready="
      << (lane_b_core_feature_expansion_ready ? "true" : "false")
      << ";lane-c-edge-case-compatibility-ready="
      << (lane_c_edge_case_compatibility_ready ? "true" : "false")
      << ";lane-d-recovery-determinism-ready="
      << (lane_d_recovery_determinism_ready ? "true" : "false")
      << ";lane-d-recovery-determinism-key-ready="
      << (lane_d_recovery_determinism_key_ready ? "true" : "false")
      << ";conformance-corpus-consistent="
      << (surface.conformance_corpus_consistent ? "true" : "false")
      << ";conformance-corpus-ready="
      << (surface.conformance_corpus_ready ? "true" : "false");
  return key.str();
}

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
