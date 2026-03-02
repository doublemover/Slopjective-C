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
  surface.core_feature_key =
      BuildObjc3FinalReadinessGateCoreFeatureImplementationKey(surface);
  surface.core_feature_impl_ready =
      surface.core_feature_impl_ready &&
      surface.core_feature_expansion_ready &&
      surface.edge_case_compatibility_ready &&
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
