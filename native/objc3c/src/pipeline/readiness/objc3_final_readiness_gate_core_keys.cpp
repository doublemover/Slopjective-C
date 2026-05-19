#include <sstream>
#include <string>

#include "pipeline/readiness/objc3_final_readiness_gate_core_keys.h"

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
