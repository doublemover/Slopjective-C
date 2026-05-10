#pragma once

#include "pipeline/readiness/objc3_final_readiness_gate_failure_reasons.h"

namespace objc3_final_readiness_gate_failure_reason_detail {

const char *FindObjc3FinalReadinessGateFoundationFailureReason(
    const Objc3FinalReadinessGateCoreFeatureImplementationSurface &surface,
    const Objc3FinalReadinessGateFailureReasonInputs &inputs);

const char *FindObjc3FinalReadinessGateAdvancedShard1AndCore2FailureReason(
    const Objc3FinalReadinessGateCoreFeatureImplementationSurface &surface,
    const Objc3FinalReadinessGateFailureReasonInputs &inputs);

const char *FindObjc3FinalReadinessGateAdvancedShard3And4FailureReason(
    const Objc3FinalReadinessGateCoreFeatureImplementationSurface &surface,
    const Objc3FinalReadinessGateFailureReasonInputs &inputs);

const char *FindObjc3FinalReadinessGateShard2TailAndSignoffFailureReason(
    const Objc3FinalReadinessGateCoreFeatureImplementationSurface &surface,
    const Objc3FinalReadinessGateFailureReasonInputs &inputs);

}  // namespace objc3_final_readiness_gate_failure_reason_detail
