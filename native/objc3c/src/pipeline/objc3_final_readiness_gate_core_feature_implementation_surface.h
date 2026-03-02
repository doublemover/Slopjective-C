#pragma once

#include <sstream>
#include <string>

#include "pipeline/objc3_frontend_types.h"

struct Objc3FinalReadinessGateCoreFeatureScaffold {
  bool governance_contract_ready = false;
  bool modular_split_ready = false;
  std::string governance_key;
  std::string modular_split_key;
};

inline std::string BuildObjc3FinalReadinessGateCoreFeatureImplementationKey(
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
      << ";core_feature_impl_ready="
      << (surface.core_feature_impl_ready ? "true" : "false");
  return key.str();
}

inline std::string BuildObjc3FinalReadinessGateEdgeCaseCompatibilityKey(
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

inline std::string BuildObjc3FinalReadinessGateEdgeCaseRobustnessKey(
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

inline std::string BuildObjc3FinalReadinessGateDiagnosticsHardeningKey(
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

inline std::string BuildObjc3FinalReadinessGateRecoveryDeterminismKey(
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

inline std::string BuildObjc3FinalReadinessGateConformanceMatrixKey(
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

inline std::string BuildObjc3FinalReadinessGateConformanceCorpusKey(
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

inline std::string BuildObjc3FinalReadinessGatePerformanceQualityGuardrailsKey(
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

inline std::string BuildObjc3FinalReadinessGateCrossLaneIntegrationKey(
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

inline std::string BuildObjc3FinalReadinessGateDocsRunbookSyncKey(
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

inline Objc3FinalReadinessGateCoreFeatureImplementationSurface
BuildObjc3FinalReadinessGateCoreFeatureImplementationSurface(
    const Objc3FinalReadinessGateCoreFeatureScaffold &scaffold,
    const Objc3FrontendLongTailGrammarCoreFeatureSurface &lane_a_surface,
    const Objc3SemanticStabilityCoreFeatureImplementationSurface &lane_b_surface,
    const Objc3LoweringRuntimeStabilityCoreFeatureImplementationSurface &lane_c_surface,
    const Objc3ToolchainRuntimeGaOperationsCoreFeatureExpansionSurface &lane_d_surface) {
  Objc3FinalReadinessGateCoreFeatureImplementationSurface surface;
  surface.governance_contract_ready = scaffold.governance_contract_ready;
  surface.modular_split_ready = scaffold.modular_split_ready;
  surface.lane_a_core_feature_ready = lane_a_surface.core_feature_ready;
  surface.lane_b_core_feature_ready = lane_b_surface.core_feature_impl_ready;
  surface.lane_c_core_feature_ready = lane_c_surface.core_feature_impl_ready;
  surface.lane_d_core_feature_ready = lane_d_surface.core_feature_impl_ready;
  surface.governance_key = scaffold.governance_key;
  surface.modular_split_key = scaffold.modular_split_key;
  surface.lane_a_key = lane_a_surface.core_feature_key;
  surface.lane_b_key = lane_b_surface.core_feature_key;
  surface.lane_c_key = lane_c_surface.core_feature_key;
  surface.lane_d_key = lane_d_surface.core_feature_key;

  const bool upstream_lanes_ready =
      surface.lane_a_core_feature_ready &&
      surface.lane_b_core_feature_ready &&
      surface.lane_c_core_feature_ready &&
      surface.lane_d_core_feature_ready;
  const bool replay_keys_ready =
      !surface.governance_key.empty() &&
      !surface.modular_split_key.empty() &&
      !surface.lane_a_key.empty() &&
      !surface.lane_b_key.empty() &&
      !surface.lane_c_key.empty() &&
      !surface.lane_d_key.empty();

  surface.dependency_chain_ready =
      surface.governance_contract_ready &&
      surface.modular_split_ready &&
      upstream_lanes_ready &&
      replay_keys_ready;
  surface.core_feature_impl_ready = surface.dependency_chain_ready;
  const bool lane_expansion_consistent =
      lane_a_surface.expansion_ready &&
      lane_b_surface.expansion_ready &&
      lane_c_surface.expansion_ready &&
      lane_d_surface.core_feature_expansion_ready;
  const bool core_feature_expansion_consistent =
      surface.core_feature_impl_ready &&
      lane_expansion_consistent;
  const bool core_feature_expansion_ready =
      core_feature_expansion_consistent &&
      !surface.governance_key.empty() &&
      !surface.modular_split_key.empty();
  surface.core_feature_expansion_consistent = core_feature_expansion_consistent;
  surface.core_feature_expansion_ready = core_feature_expansion_ready;
  surface.core_feature_key =
      BuildObjc3FinalReadinessGateCoreFeatureImplementationKey(surface);
  surface.core_feature_expansion_key =
      "final-readiness-gate-core-feature-expansion:v1:"
      "dependency-chain-ready=" +
      std::string(surface.dependency_chain_ready ? "true" : "false") +
      ";lane-a-expansion-ready=" +
      std::string(lane_a_surface.expansion_ready ? "true" : "false") +
      ";lane-b-expansion-ready=" +
      std::string(lane_b_surface.expansion_ready ? "true" : "false") +
      ";lane-c-expansion-ready=" +
      std::string(lane_c_surface.expansion_ready ? "true" : "false") +
      ";lane-d-core-feature-expansion-ready=" +
      std::string(lane_d_surface.core_feature_expansion_ready ? "true" : "false") +
      ";core-feature-expansion-consistent=" +
      std::string(core_feature_expansion_consistent ? "true" : "false") +
      ";core-feature-expansion-ready=" +
      std::string(core_feature_expansion_ready ? "true" : "false");

  const bool lane_edge_case_compatibility_consistent =
      lane_a_surface.edge_case_compatibility_ready &&
      lane_b_surface.edge_case_compatibility_ready &&
      lane_c_surface.edge_case_compatibility_ready &&
      lane_d_surface.edge_case_compatibility_ready;
  const bool edge_case_compatibility_consistent =
      surface.core_feature_expansion_ready &&
      lane_edge_case_compatibility_consistent;
  const bool edge_case_compatibility_ready =
      edge_case_compatibility_consistent &&
      !surface.governance_key.empty() &&
      !surface.modular_split_key.empty() &&
      !surface.core_feature_expansion_key.empty();

  surface.core_feature_expansion_ready =
      surface.core_feature_expansion_ready &&
      !surface.core_feature_expansion_key.empty();
  surface.edge_case_compatibility_consistent =
      edge_case_compatibility_consistent;
  surface.edge_case_compatibility_ready =
      edge_case_compatibility_ready;
  surface.edge_case_compatibility_key =
      BuildObjc3FinalReadinessGateEdgeCaseCompatibilityKey(
          surface,
          lane_a_surface.edge_case_compatibility_ready,
          lane_b_surface.edge_case_compatibility_ready,
          lane_c_surface.edge_case_compatibility_ready,
          lane_d_surface.edge_case_compatibility_ready);
  surface.edge_case_compatibility_ready =
      surface.edge_case_compatibility_ready &&
      !surface.edge_case_compatibility_key.empty();

  const bool lane_edge_case_expansion_consistent =
      lane_a_surface.core_feature_ready &&
      lane_b_surface.core_feature_impl_ready &&
      lane_c_surface.core_feature_impl_ready &&
      lane_d_surface.edge_case_compatibility_ready;
  const bool edge_case_expansion_consistent =
      surface.edge_case_compatibility_ready &&
      lane_edge_case_expansion_consistent;
  const bool edge_case_robustness_ready =
      edge_case_expansion_consistent &&
      !surface.governance_key.empty() &&
      !surface.modular_split_key.empty() &&
      !surface.edge_case_compatibility_key.empty();
  surface.edge_case_expansion_consistent =
      edge_case_expansion_consistent;
  surface.edge_case_robustness_ready =
      edge_case_robustness_ready;
  surface.edge_case_robustness_key =
      BuildObjc3FinalReadinessGateEdgeCaseRobustnessKey(
          surface,
          lane_a_surface.core_feature_ready,
          lane_b_surface.core_feature_impl_ready,
          lane_c_surface.core_feature_impl_ready,
          lane_d_surface.edge_case_compatibility_ready);
  surface.edge_case_robustness_ready =
      surface.edge_case_robustness_ready &&
      !surface.edge_case_robustness_key.empty();
  const bool lane_diagnostics_hardening_consistent =
      lane_a_surface.core_feature_ready &&
      lane_b_surface.core_feature_impl_ready &&
      lane_c_surface.core_feature_impl_ready &&
      lane_d_surface.edge_case_robustness_ready;
  const bool diagnostics_hardening_consistent =
      surface.edge_case_robustness_ready &&
      lane_diagnostics_hardening_consistent;
  const bool diagnostics_hardening_ready =
      diagnostics_hardening_consistent &&
      !surface.governance_key.empty() &&
      !surface.modular_split_key.empty() &&
      !surface.edge_case_robustness_key.empty();
  surface.diagnostics_hardening_consistent =
      diagnostics_hardening_consistent;
  surface.diagnostics_hardening_ready =
      diagnostics_hardening_ready;
  surface.diagnostics_hardening_key =
      BuildObjc3FinalReadinessGateDiagnosticsHardeningKey(
          surface,
          lane_a_surface.core_feature_ready,
          lane_b_surface.core_feature_impl_ready,
          lane_c_surface.core_feature_impl_ready,
          lane_d_surface.edge_case_robustness_ready);
  surface.diagnostics_hardening_ready =
      surface.diagnostics_hardening_ready &&
      !surface.diagnostics_hardening_key.empty();
  const bool lane_recovery_determinism_consistent =
      lane_a_surface.core_feature_ready &&
      lane_b_surface.expansion_ready &&
      lane_c_surface.expansion_ready &&
      lane_d_surface.diagnostics_hardening_ready;
  const bool recovery_determinism_consistent =
      surface.diagnostics_hardening_ready &&
      lane_recovery_determinism_consistent;
  const bool recovery_determinism_ready =
      recovery_determinism_consistent &&
      !surface.governance_key.empty() &&
      !surface.modular_split_key.empty() &&
      !surface.diagnostics_hardening_key.empty();
  surface.recovery_determinism_consistent =
      recovery_determinism_consistent;
  surface.recovery_determinism_ready =
      recovery_determinism_ready;
  surface.recovery_determinism_key =
      BuildObjc3FinalReadinessGateRecoveryDeterminismKey(
          surface,
          lane_a_surface.core_feature_ready,
          lane_b_surface.expansion_ready,
          lane_c_surface.expansion_ready,
          lane_d_surface.diagnostics_hardening_ready);
  surface.recovery_determinism_ready =
      surface.recovery_determinism_ready &&
      !surface.recovery_determinism_key.empty();
  const bool lane_conformance_matrix_consistent =
      lane_a_surface.core_feature_ready &&
      lane_b_surface.expansion_ready &&
      lane_c_surface.expansion_ready &&
      lane_d_surface.diagnostics_hardening_ready;
  const bool conformance_matrix_consistent =
      surface.recovery_determinism_ready &&
      lane_conformance_matrix_consistent;
  const bool conformance_matrix_ready =
      conformance_matrix_consistent &&
      !surface.governance_key.empty() &&
      !surface.modular_split_key.empty() &&
      !surface.recovery_determinism_key.empty();
  surface.conformance_matrix_consistent =
      conformance_matrix_consistent;
  surface.conformance_matrix_ready =
      conformance_matrix_ready;
  surface.conformance_matrix_key =
      BuildObjc3FinalReadinessGateConformanceMatrixKey(
          surface,
          lane_a_surface.core_feature_ready,
          lane_b_surface.expansion_ready,
          lane_c_surface.expansion_ready,
          lane_d_surface.diagnostics_hardening_ready);
  surface.conformance_matrix_ready =
      surface.conformance_matrix_ready &&
      !surface.conformance_matrix_key.empty();
  const bool lane_conformance_corpus_consistent =
      lane_a_surface.core_feature_expansion_ready &&
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
          lane_a_surface.core_feature_expansion_ready,
          lane_b_surface.expansion_ready,
          lane_c_surface.edge_case_compatibility_ready,
          lane_d_surface.recovery_determinism_ready,
          !lane_d_surface.recovery_determinism_key.empty());
  surface.conformance_corpus_ready =
      surface.conformance_corpus_ready &&
      !surface.conformance_corpus_key.empty();
  const bool lane_performance_quality_guardrails_consistent =
      lane_a_surface.core_feature_expansion_ready &&
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
          lane_a_surface.core_feature_expansion_ready,
          lane_b_surface.edge_case_compatibility_ready,
          lane_c_surface.edge_case_compatibility_ready,
          lane_d_surface.conformance_matrix_ready,
          !lane_d_surface.conformance_matrix_key.empty());
  surface.performance_quality_guardrails_ready =
      surface.performance_quality_guardrails_ready &&
      !surface.performance_quality_guardrails_key.empty();
  const bool lane_cross_lane_integration_consistent =
      lane_a_surface.core_feature_expansion_ready &&
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
          lane_a_surface.core_feature_expansion_ready,
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
      !surface.core_feature_key.empty();

  if (surface.core_feature_impl_ready) {
    return surface;
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
  } else if (!replay_keys_ready) {
    surface.failure_reason =
        "final readiness gate dependency replay keys are not ready";
  } else if (!lane_expansion_consistent) {
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
  } else if (!lane_edge_case_compatibility_consistent) {
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
  } else if (!lane_edge_case_expansion_consistent) {
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
  } else if (!lane_diagnostics_hardening_consistent) {
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
  } else if (!lane_recovery_determinism_consistent) {
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
  } else if (!lane_conformance_matrix_consistent) {
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
  } else if (!lane_conformance_corpus_consistent) {
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
  } else if (!lane_performance_quality_guardrails_consistent) {
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
  } else if (!lane_cross_lane_integration_consistent) {
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
  } else if (!lane_docs_runbook_sync_consistent) {
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
  } else {
    surface.failure_reason =
        "final readiness gate core feature implementation is not ready";
  }

  return surface;
}

inline bool IsObjc3FinalReadinessGateCoreFeatureImplementationSurfaceReady(
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
