#pragma once

#include "pipeline/objc3_final_readiness_gate_surface_readiness_helpers.h"

namespace objc3_final_readiness_gate_surface {

void PublishObjc3FinalReadinessGateTailReadiness(
    Objc3FinalReadinessGateCoreFeatureImplementationSurface &surface,
    const Objc3FinalReadinessGateLaneSurface &lane_a_surface,
    const Objc3FinalReadinessGateLaneSurface &lane_b_surface,
    const Objc3FinalReadinessGateLaneSurface &lane_c_surface,
    const Objc3FinalReadinessGateLaneSurface &lane_d_surface);

}  // namespace objc3_final_readiness_gate_surface
