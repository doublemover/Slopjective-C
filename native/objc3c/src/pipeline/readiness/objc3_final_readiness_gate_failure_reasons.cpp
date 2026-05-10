#include "pipeline/readiness/objc3_final_readiness_gate_failure_reasons.h"

#include <string>

#include "pipeline/readiness/objc3_final_readiness_gate_failure_reasons_private.h"

void ApplyObjc3FinalReadinessGateFailureReason(
    Objc3FinalReadinessGateCoreFeatureImplementationSurface &surface,
    const Objc3FinalReadinessGateFailureReasonInputs &inputs) {
  if (surface.core_feature_impl_ready) {
    return;
  }

  const char *failure_reason =
      objc3_final_readiness_gate_failure_reason_detail::
          FindObjc3FinalReadinessGateFoundationFailureReason(surface, inputs);
  if (failure_reason == nullptr) {
    failure_reason =
        objc3_final_readiness_gate_failure_reason_detail::
            FindObjc3FinalReadinessGateAdvancedShard1AndCore2FailureReason(
                surface,
                inputs);
  }
  if (failure_reason == nullptr) {
    failure_reason =
        objc3_final_readiness_gate_failure_reason_detail::
            FindObjc3FinalReadinessGateAdvancedShard3And4FailureReason(
                surface,
                inputs);
  }
  if (failure_reason == nullptr) {
    failure_reason =
        objc3_final_readiness_gate_failure_reason_detail::
            FindObjc3FinalReadinessGateShard2TailAndSignoffFailureReason(
                surface,
                inputs);
  }

  surface.failure_reason =
      failure_reason == nullptr
          ? "final readiness gate core feature implementation is not ready"
          : failure_reason;
}

bool IsObjc3FinalReadinessGateCoreFeatureImplementationSurfaceReady(
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
