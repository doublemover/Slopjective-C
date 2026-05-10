#include "pipeline/readiness/objc3_final_readiness_gate_failure_reasons_private.h"

namespace objc3_final_readiness_gate_failure_reason_detail {

const char *FindObjc3FinalReadinessGateShard2TailAndSignoffFailureReason(
    const Objc3FinalReadinessGateCoreFeatureImplementationSurface &surface,
    const Objc3FinalReadinessGateFailureReasonInputs &inputs) {
  if (!inputs.lane_advanced_edge_compatibility_shard2_consistent) {
    return "final readiness gate advanced edge compatibility workpack shard2 is inconsistent";
  }

  if (!surface.advanced_edge_compatibility_shard2_consistent) {
    return "final readiness gate advanced edge compatibility workpack shard2 consistency is not satisfied";
  }

  if (!surface.advanced_edge_compatibility_shard2_ready) {
    return "final readiness gate advanced edge compatibility workpack shard2 is not ready";
  }

  if (surface.advanced_edge_compatibility_shard2_key.empty()) {
    return "final readiness gate advanced edge compatibility workpack shard2 key is not ready";
  }

  if (!inputs.lane_advanced_diagnostics_shard2_consistent) {
    return "final readiness gate advanced diagnostics workpack shard2 is inconsistent";
  }

  if (!surface.advanced_diagnostics_shard2_consistent) {
    return "final readiness gate advanced diagnostics workpack shard2 consistency is not satisfied";
  }

  if (!surface.advanced_diagnostics_shard2_ready) {
    return "final readiness gate advanced diagnostics workpack shard2 is not ready";
  }

  if (surface.advanced_diagnostics_shard2_key.empty()) {
    return "final readiness gate advanced diagnostics workpack shard2 key is not ready";
  }

  if (!inputs.lane_advanced_conformance_shard2_consistent) {
    return "final readiness gate advanced conformance workpack shard2 is inconsistent";
  }

  if (!surface.advanced_conformance_shard2_consistent) {
    return "final readiness gate advanced conformance workpack shard2 consistency is not satisfied";
  }

  if (!surface.advanced_conformance_shard2_ready) {
    return "final readiness gate advanced conformance workpack shard2 is not ready";
  }

  if (surface.advanced_conformance_shard2_key.empty()) {
    return "final readiness gate advanced conformance workpack shard2 key is not ready";
  }

  if (!inputs.lane_advanced_integration_shard2_consistent) {
    return "final readiness gate advanced integration workpack shard2 is inconsistent";
  }

  if (!surface.advanced_integration_shard2_consistent) {
    return "final readiness gate advanced integration workpack shard2 consistency is not satisfied";
  }

  if (!surface.advanced_integration_shard2_ready) {
    return "final readiness gate advanced integration workpack shard2 is not ready";
  }

  if (surface.advanced_integration_shard2_key.empty()) {
    return "final readiness gate advanced integration workpack shard2 key is not ready";
  }

  if (!inputs.lane_advanced_performance_shard2_consistent) {
    return "final readiness gate advanced performance workpack shard2 is inconsistent";
  }

  if (!surface.advanced_performance_shard2_consistent) {
    return "final readiness gate advanced performance workpack shard2 consistency is not satisfied";
  }

  if (!surface.advanced_performance_shard2_ready) {
    return "final readiness gate advanced performance workpack shard2 is not ready";
  }

  if (surface.advanced_performance_shard2_key.empty()) {
    return "final readiness gate advanced performance workpack shard2 key is not ready";
  }

  if (!inputs.lane_integration_closeout_signoff_consistent) {
    return "final readiness gate integration closeout and gate sign-off is inconsistent";
  }

  if (!surface.integration_closeout_signoff_consistent) {
    return "final readiness gate integration closeout and gate sign-off consistency is not satisfied";
  }

  if (!surface.integration_closeout_signoff_ready) {
    return "final readiness gate integration closeout and gate sign-off is not ready";
  }

  if (surface.integration_closeout_signoff_key.empty()) {
    return "final readiness gate integration closeout and gate sign-off key is not ready";
  }

  return nullptr;
}

}  // namespace objc3_final_readiness_gate_failure_reason_detail
