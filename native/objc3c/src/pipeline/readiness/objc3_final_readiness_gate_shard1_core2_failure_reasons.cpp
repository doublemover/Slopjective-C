#include "pipeline/readiness/objc3_final_readiness_gate_failure_reasons_private.h"

namespace objc3_final_readiness_gate_failure_reason_detail {

const char *FindObjc3FinalReadinessGateAdvancedShard1AndCore2FailureReason(
    const Objc3FinalReadinessGateCoreFeatureImplementationSurface &surface,
    const Objc3FinalReadinessGateFailureReasonInputs &inputs) {
  if (!inputs.lane_advanced_core_shard1_consistent) {
    return "final readiness gate advanced core workpack shard1 is inconsistent";
  }

  if (!surface.advanced_core_shard1_consistent) {
    return "final readiness gate advanced core workpack shard1 consistency is not satisfied";
  }

  if (!surface.advanced_core_shard1_ready) {
    return "final readiness gate advanced core workpack shard1 is not ready";
  }

  if (surface.advanced_core_shard1_key.empty()) {
    return "final readiness gate advanced core workpack shard1 key is not ready";
  }

  if (!inputs.lane_advanced_edge_compatibility_shard1_consistent) {
    return "final readiness gate advanced edge compatibility workpack shard1 is inconsistent";
  }

  if (!surface.advanced_edge_compatibility_shard1_consistent) {
    return "final readiness gate advanced edge compatibility workpack shard1 consistency is not satisfied";
  }

  if (!surface.advanced_edge_compatibility_shard1_ready) {
    return "final readiness gate advanced edge compatibility workpack shard1 is not ready";
  }

  if (surface.advanced_edge_compatibility_shard1_key.empty()) {
    return "final readiness gate advanced edge compatibility workpack shard1 key is not ready";
  }

  if (!inputs.lane_advanced_diagnostics_shard1_consistent) {
    return "final readiness gate advanced diagnostics workpack shard1 is inconsistent";
  }

  if (!surface.advanced_diagnostics_shard1_consistent) {
    return "final readiness gate advanced diagnostics workpack shard1 consistency is not satisfied";
  }

  if (!surface.advanced_diagnostics_shard1_ready) {
    return "final readiness gate advanced diagnostics workpack shard1 is not ready";
  }

  if (surface.advanced_diagnostics_shard1_key.empty()) {
    return "final readiness gate advanced diagnostics workpack shard1 key is not ready";
  }

  if (!inputs.lane_advanced_conformance_shard1_consistent) {
    return "final readiness gate advanced conformance workpack shard1 is inconsistent";
  }

  if (!surface.advanced_conformance_shard1_consistent) {
    return "final readiness gate advanced conformance workpack shard1 consistency is not satisfied";
  }

  if (!surface.advanced_conformance_shard1_ready) {
    return "final readiness gate advanced conformance workpack shard1 is not ready";
  }

  if (surface.advanced_conformance_shard1_key.empty()) {
    return "final readiness gate advanced conformance workpack shard1 key is not ready";
  }

  if (!inputs.lane_advanced_integration_shard1_consistent) {
    return "final readiness gate advanced integration workpack shard1 is inconsistent";
  }

  if (!surface.advanced_integration_shard1_consistent) {
    return "final readiness gate advanced integration workpack shard1 consistency is not satisfied";
  }

  if (!surface.advanced_integration_shard1_ready) {
    return "final readiness gate advanced integration workpack shard1 is not ready";
  }

  if (surface.advanced_integration_shard1_key.empty()) {
    return "final readiness gate advanced integration workpack shard1 key is not ready";
  }

  if (!inputs.lane_advanced_performance_shard1_consistent) {
    return "final readiness gate advanced performance workpack shard1 is inconsistent";
  }

  if (!surface.advanced_performance_shard1_consistent) {
    return "final readiness gate advanced performance workpack shard1 consistency is not satisfied";
  }

  if (!surface.advanced_performance_shard1_ready) {
    return "final readiness gate advanced performance workpack shard1 is not ready";
  }

  if (surface.advanced_performance_shard1_key.empty()) {
    return "final readiness gate advanced performance workpack shard1 key is not ready";
  }

  if (!inputs.lane_advanced_core_shard2_consistent) {
    return "final readiness gate advanced core workpack shard2 is inconsistent";
  }

  if (!surface.advanced_core_shard2_consistent) {
    return "final readiness gate advanced core workpack shard2 consistency is not satisfied";
  }

  if (!surface.advanced_core_shard2_ready) {
    return "final readiness gate advanced core workpack shard2 is not ready";
  }

  if (surface.advanced_core_shard2_key.empty()) {
    return "final readiness gate advanced core workpack shard2 key is not ready";
  }

  return nullptr;
}

}  // namespace objc3_final_readiness_gate_failure_reason_detail
