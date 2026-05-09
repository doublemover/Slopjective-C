#pragma once

#include <string>

#include "pipeline/objc3_frontend_types.h"

struct Objc3FinalReadinessGateCoreFeatureScaffold {
  bool governance_contract_ready = false;
  bool modular_split_ready = false;
  std::string governance_key;
  std::string modular_split_key;
};

std::string BuildObjc3FinalReadinessGateCoreFeatureImplementationKey(
    const Objc3FinalReadinessGateCoreFeatureImplementationSurface &surface);

std::string BuildObjc3FinalReadinessGateEdgeCaseCompatibilityKey(
    const Objc3FinalReadinessGateCoreFeatureImplementationSurface &surface,
    bool lane_a_edge_case_compatibility_ready,
    bool lane_b_edge_case_compatibility_ready,
    bool lane_c_edge_case_compatibility_ready,
    bool lane_d_edge_case_compatibility_ready);

std::string BuildObjc3FinalReadinessGateEdgeCaseRobustnessKey(
    const Objc3FinalReadinessGateCoreFeatureImplementationSurface &surface,
    bool lane_a_core_feature_ready,
    bool lane_b_core_feature_ready,
    bool lane_c_core_feature_ready,
    bool lane_d_edge_case_compatibility_ready);

std::string BuildObjc3FinalReadinessGateDiagnosticsHardeningKey(
    const Objc3FinalReadinessGateCoreFeatureImplementationSurface &surface,
    bool lane_a_core_feature_ready,
    bool lane_b_core_feature_ready,
    bool lane_c_core_feature_ready,
    bool lane_d_edge_case_robustness_ready);

std::string BuildObjc3FinalReadinessGateRecoveryDeterminismKey(
    const Objc3FinalReadinessGateCoreFeatureImplementationSurface &surface,
    bool lane_a_core_feature_ready,
    bool lane_b_core_feature_expansion_ready,
    bool lane_c_core_feature_expansion_ready,
    bool lane_d_diagnostics_hardening_ready);

std::string BuildObjc3FinalReadinessGateConformanceMatrixKey(
    const Objc3FinalReadinessGateCoreFeatureImplementationSurface &surface,
    bool lane_a_core_feature_ready,
    bool lane_b_core_feature_expansion_ready,
    bool lane_c_core_feature_expansion_ready,
    bool lane_d_diagnostics_hardening_ready);

std::string BuildObjc3FinalReadinessGateConformanceCorpusKey(
    const Objc3FinalReadinessGateCoreFeatureImplementationSurface &surface,
    bool lane_a_core_feature_expansion_ready,
    bool lane_b_core_feature_expansion_ready,
    bool lane_c_edge_case_compatibility_ready,
    bool lane_d_recovery_determinism_ready,
    bool lane_d_recovery_determinism_key_ready);

std::string BuildObjc3FinalReadinessGatePerformanceQualityGuardrailsKey(
    const Objc3FinalReadinessGateCoreFeatureImplementationSurface &surface,
    bool lane_a_core_feature_expansion_ready,
    bool lane_b_edge_case_compatibility_ready,
    bool lane_c_edge_case_compatibility_ready,
    bool lane_d_conformance_matrix_ready,
    bool lane_d_conformance_matrix_key_ready);

std::string BuildObjc3FinalReadinessGateCrossLaneIntegrationKey(
    const Objc3FinalReadinessGateCoreFeatureImplementationSurface &surface,
    bool lane_a_core_feature_expansion_ready,
    bool lane_b_edge_case_compatibility_ready,
    bool lane_c_edge_case_robustness_ready,
    bool lane_d_conformance_matrix_ready,
    bool lane_d_conformance_matrix_key_ready);

std::string BuildObjc3FinalReadinessGateDocsRunbookSyncKey(
    const Objc3FinalReadinessGateCoreFeatureImplementationSurface &surface,
    bool lane_a_edge_case_compatibility_ready,
    bool lane_b_edge_case_robustness_ready,
    bool lane_c_edge_case_robustness_ready,
    bool lane_d_conformance_matrix_ready,
    bool lane_d_conformance_matrix_key_ready);

std::string BuildObjc3FinalReadinessGateReleaseCandidateReplayDryRunKey(
    const Objc3FinalReadinessGateCoreFeatureImplementationSurface &surface,
    bool lane_a_edge_case_compatibility_ready,
    bool lane_b_edge_case_robustness_ready,
    bool lane_c_diagnostics_hardening_ready,
    bool lane_d_core_feature_impl_ready,
    bool lane_d_core_feature_key_ready);
