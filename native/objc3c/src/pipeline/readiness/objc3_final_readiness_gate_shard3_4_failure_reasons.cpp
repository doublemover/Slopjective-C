#include "pipeline/readiness/objc3_final_readiness_gate_failure_reasons_private.h"

namespace objc3_final_readiness_gate_failure_reason_detail {

const char *FindObjc3FinalReadinessGateAdvancedShard3And4FailureReason(
    const Objc3FinalReadinessGateCoreFeatureImplementationSurface &surface,
    const Objc3FinalReadinessGateFailureReasonInputs &inputs) {
  if (!inputs.lane_advanced_core_shard3_consistent) {
    return "final readiness gate advanced core workpack shard3 is inconsistent";
  }

  if (!surface.advanced_core_shard3_consistent) {
    return "final readiness gate advanced core workpack shard3 consistency is not satisfied";
  }

  if (!surface.advanced_core_shard3_ready) {
    return "final readiness gate advanced core workpack shard3 is not ready";
  }

  if (surface.advanced_core_shard3_key.empty()) {
    return "final readiness gate advanced core workpack shard3 key is not ready";
  }

  if (!inputs.lane_advanced_edge_compatibility_shard3_consistent) {
    return "final readiness gate advanced edge compatibility workpack shard3 is inconsistent";
  }

  if (!surface.advanced_edge_compatibility_shard3_consistent) {
    return "final readiness gate advanced edge compatibility workpack shard3 consistency is not satisfied";
  }

  if (!surface.advanced_edge_compatibility_shard3_ready) {
    return "final readiness gate advanced edge compatibility workpack shard3 is not ready";
  }

  if (surface.advanced_edge_compatibility_shard3_key.empty()) {
    return "final readiness gate advanced edge compatibility workpack shard3 key is not ready";
  }

  if (!inputs.lane_advanced_diagnostics_shard3_consistent) {
    return "final readiness gate advanced diagnostics workpack shard3 is inconsistent";
  }

  if (!surface.advanced_diagnostics_shard3_consistent) {
    return "final readiness gate advanced diagnostics workpack shard3 consistency is not satisfied";
  }

  if (!surface.advanced_diagnostics_shard3_ready) {
    return "final readiness gate advanced diagnostics workpack shard3 is not ready";
  }

  if (surface.advanced_diagnostics_shard3_key.empty()) {
    return "final readiness gate advanced diagnostics workpack shard3 key is not ready";
  }

  if (!inputs.lane_advanced_conformance_shard3_consistent) {
    return "final readiness gate advanced conformance workpack shard3 is inconsistent";
  }

  if (!surface.advanced_conformance_shard3_consistent) {
    return "final readiness gate advanced conformance workpack shard3 consistency is not satisfied";
  }

  if (!surface.advanced_conformance_shard3_ready) {
    return "final readiness gate advanced conformance workpack shard3 is not ready";
  }

  if (surface.advanced_conformance_shard3_key.empty()) {
    return "final readiness gate advanced conformance workpack shard3 key is not ready";
  }

  if (!inputs.lane_advanced_integration_shard3_consistent) {
    return "final readiness gate advanced integration workpack shard3 is inconsistent";
  }

  if (!surface.advanced_integration_shard3_consistent) {
    return "final readiness gate advanced integration workpack shard3 consistency is not satisfied";
  }

  if (!surface.advanced_integration_shard3_ready) {
    return "final readiness gate advanced integration workpack shard3 is not ready";
  }

  if (surface.advanced_integration_shard3_key.empty()) {
    return "final readiness gate advanced integration workpack shard3 key is not ready";
  }

  if (!inputs.lane_advanced_performance_shard3_consistent) {
    return "final readiness gate advanced performance workpack shard3 is inconsistent";
  }

  if (!surface.advanced_performance_shard3_consistent) {
    return "final readiness gate advanced performance workpack shard3 consistency is not satisfied";
  }

  if (!surface.advanced_performance_shard3_ready) {
    return "final readiness gate advanced performance workpack shard3 is not ready";
  }

  if (surface.advanced_performance_shard3_key.empty()) {
    return "final readiness gate advanced performance workpack shard3 key is not ready";
  }

  if (!inputs.lane_advanced_core_shard4_consistent) {
    return "final readiness gate advanced core workpack shard4 is inconsistent";
  }

  if (!surface.advanced_core_shard4_consistent) {
    return "final readiness gate advanced core workpack shard4 consistency is not satisfied";
  }

  if (!surface.advanced_core_shard4_ready) {
    return "final readiness gate advanced core workpack shard4 is not ready";
  }

  if (surface.advanced_core_shard4_key.empty()) {
    return "final readiness gate advanced core workpack shard4 key is not ready";
  }

  if (!inputs.lane_advanced_edge_compatibility_shard4_consistent) {
    return "final readiness gate advanced edge compatibility workpack shard4 is inconsistent";
  }

  if (!surface.advanced_edge_compatibility_shard4_consistent) {
    return "final readiness gate advanced edge compatibility workpack shard4 consistency is not satisfied";
  }

  if (!surface.advanced_edge_compatibility_shard4_ready) {
    return "final readiness gate advanced edge compatibility workpack shard4 is not ready";
  }

  if (surface.advanced_edge_compatibility_shard4_key.empty()) {
    return "final readiness gate advanced edge compatibility workpack shard4 key is not ready";
  }

  if (!inputs.lane_advanced_integration_closeout_signoff_consistent) {
    return "final readiness gate integration closeout and gate sign-off is inconsistent";
  }

  if (!surface.advanced_integration_closeout_signoff_consistent) {
    return "final readiness gate integration closeout and gate sign-off consistency is not satisfied";
  }

  if (!surface.advanced_integration_closeout_signoff_ready) {
    return "final readiness gate integration closeout and gate sign-off is not ready";
  }

  if (surface.advanced_integration_closeout_signoff_key.empty()) {
    return "final readiness gate integration closeout and gate sign-off key is not ready";
  }

  return nullptr;
}

}  // namespace objc3_final_readiness_gate_failure_reason_detail
