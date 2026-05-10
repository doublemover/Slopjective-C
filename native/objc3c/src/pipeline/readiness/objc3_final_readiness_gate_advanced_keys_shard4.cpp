#include <sstream>
#include <string>

#include "pipeline/readiness/objc3_final_readiness_gate_advanced_keys.h"

std::string BuildObjc3FinalReadinessGateAdvancedCoreShard4Key(
    const Objc3FinalReadinessGateCoreFeatureImplementationSurface &surface,
    bool lane_a_cross_lane_integration_ready,
    bool lane_b_advanced_core_shard1_ready,
    bool lane_c_advanced_conformance_shard1_ready,
    bool lane_d_advanced_conformance_shard2_ready,
    bool lane_d_advanced_conformance_shard2_key_ready) {
  std::ostringstream key;
  key << "final-readiness-gate-advanced-core-shard4:v1:"
      << "dependency-chain-ready="
      << (surface.dependency_chain_ready ? "true" : "false")
      << ";advanced-performance-shard3-ready="
      << (surface.advanced_performance_shard3_ready ? "true" : "false")
      << ";lane-a-cross-lane-integration-ready="
      << (lane_a_cross_lane_integration_ready ? "true" : "false")
      << ";lane-b-advanced-core-shard1-ready="
      << (lane_b_advanced_core_shard1_ready ? "true" : "false")
      << ";lane-c-advanced-conformance-shard1-ready="
      << (lane_c_advanced_conformance_shard1_ready ? "true" : "false")
      << ";lane-d-advanced-conformance-shard2-ready="
      << (lane_d_advanced_conformance_shard2_ready ? "true" : "false")
      << ";lane-d-advanced-conformance-shard2-key-ready="
      << (lane_d_advanced_conformance_shard2_key_ready ? "true" : "false")
      << ";advanced-core-shard4-consistent="
      << (surface.advanced_core_shard4_consistent ? "true" : "false")
      << ";advanced-core-shard4-ready="
      << (surface.advanced_core_shard4_ready ? "true" : "false");
  return key.str();
}

std::string
BuildObjc3FinalReadinessGateAdvancedEdgeCompatibilityShard4Key(
    const Objc3FinalReadinessGateCoreFeatureImplementationSurface &surface,
    bool lane_a_release_candidate_replay_dry_run_ready,
    bool lane_b_integration_closeout_signoff_ready,
    bool lane_c_advanced_conformance_shard1_ready,
    bool lane_d_advanced_conformance_shard2_ready,
    bool lane_d_advanced_conformance_shard2_key_ready) {
  std::ostringstream key;
  key << "final-readiness-gate-advanced-edge-compatibility-shard4:v1:"
      << "dependency-chain-ready="
      << (surface.dependency_chain_ready ? "true" : "false")
      << ";advanced-core-shard4-ready="
      << (surface.advanced_core_shard4_ready ? "true" : "false")
      << ";lane-a-release-candidate-replay-dry-run-ready="
      << (lane_a_release_candidate_replay_dry_run_ready ? "true" : "false")
      << ";lane-b-integration-closeout-signoff-ready="
      << (lane_b_integration_closeout_signoff_ready ? "true" : "false")
      << ";lane-c-advanced-conformance-shard1-ready="
      << (lane_c_advanced_conformance_shard1_ready ? "true" : "false")
      << ";lane-d-advanced-conformance-shard2-ready="
      << (lane_d_advanced_conformance_shard2_ready ? "true" : "false")
      << ";lane-d-advanced-conformance-shard2-key-ready="
      << (lane_d_advanced_conformance_shard2_key_ready ? "true" : "false")
      << ";advanced-edge-compatibility-shard4-consistent="
      << (surface.advanced_edge_compatibility_shard4_consistent ? "true"
                                                                 : "false")
      << ";advanced-edge-compatibility-shard4-ready="
      << (surface.advanced_edge_compatibility_shard4_ready ? "true" : "false");
  return key.str();
}

std::string BuildObjc3FinalReadinessGateAdvancedIntegrationCloseoutSignoffKey(
    const Objc3FinalReadinessGateCoreFeatureImplementationSurface &surface,
    bool lane_a_integration_closeout_signoff_ready,
    bool lane_b_integration_closeout_signoff_ready,
    bool lane_c_integration_closeout_signoff_ready,
    bool lane_d_integration_closeout_signoff_ready,
    bool lane_d_integration_closeout_signoff_key_ready) {
  std::ostringstream key;
  key << "final-readiness-gate-advanced-integration-closeout-signoff:v1:"
      << "dependency-chain-ready="
      << (surface.dependency_chain_ready ? "true" : "false")
      << ";advanced-edge-compatibility-shard4-ready="
      << (surface.advanced_edge_compatibility_shard4_ready ? "true" : "false")
      << ";lane-a-integration-closeout-signoff-ready="
      << (lane_a_integration_closeout_signoff_ready ? "true" : "false")
      << ";lane-b-integration-closeout-signoff-ready="
      << (lane_b_integration_closeout_signoff_ready ? "true" : "false")
      << ";lane-c-integration-closeout-signoff-ready="
      << (lane_c_integration_closeout_signoff_ready ? "true" : "false")
      << ";lane-d-integration-closeout-signoff-ready="
      << (lane_d_integration_closeout_signoff_ready ? "true" : "false")
      << ";lane-d-integration-closeout-signoff-key-ready="
      << (lane_d_integration_closeout_signoff_key_ready ? "true" : "false")
      << ";advanced-integration-closeout-signoff-consistent="
      << (surface.advanced_integration_closeout_signoff_consistent ? "true" : "false")
      << ";advanced-integration-closeout-signoff-ready="
      << (surface.advanced_integration_closeout_signoff_ready ? "true" : "false");
  return key.str();
}
