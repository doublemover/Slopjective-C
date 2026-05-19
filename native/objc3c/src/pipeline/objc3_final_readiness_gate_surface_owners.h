#pragma once

#include "pipeline/objc3_final_readiness_gate_core_feature_implementation_surface.h"

namespace objc3_final_readiness_gate_surface {

void PopulateObjc3FinalReadinessGateSurfaceEvidence(
    Objc3FinalReadinessGateCoreFeatureImplementationSurface &surface,
    const Objc3FinalReadinessGateCoreFeatureScaffold &scaffold,
    const Objc3FinalReadinessGateLaneSurface &lane_a_surface,
    const Objc3FinalReadinessGateLaneSurface &lane_b_surface,
    const Objc3FinalReadinessGateLaneSurface &lane_c_surface,
    const Objc3FinalReadinessGateLaneSurface &lane_d_surface);

void PublishObjc3FinalReadinessGateSurfaceReadiness(
    Objc3FinalReadinessGateCoreFeatureImplementationSurface &surface,
    const Objc3FinalReadinessGateLaneSurface &lane_a_surface,
    const Objc3FinalReadinessGateLaneSurface &lane_b_surface,
    const Objc3FinalReadinessGateLaneSurface &lane_c_surface,
    const Objc3FinalReadinessGateLaneSurface &lane_d_surface);

void PublishObjc3FinalReadinessGateSurfaceFailureReasons(
    Objc3FinalReadinessGateCoreFeatureImplementationSurface &surface,
    const Objc3FinalReadinessGateLaneSurface &lane_a_surface,
    const Objc3FinalReadinessGateLaneSurface &lane_b_surface,
    const Objc3FinalReadinessGateLaneSurface &lane_c_surface,
    const Objc3FinalReadinessGateLaneSurface &lane_d_surface);

}  // namespace objc3_final_readiness_gate_surface
