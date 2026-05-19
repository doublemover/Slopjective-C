#pragma once

#include "pipeline/objc3_frontend_long_tail_grammar_core_feature_surface.h"
#include "pipeline/objc3_frontend_types.h"
#include "pipeline/readiness/objc3_final_readiness_gate_core_keys.h"

Objc3FinalReadinessGateCoreFeatureImplementationSurface
BuildObjc3FinalReadinessGateCoreFeatureImplementationSurface(
    const Objc3FinalReadinessGateCoreFeatureScaffold &scaffold,
    const Objc3FinalReadinessGateLaneSurface &lane_a_surface,
    const Objc3FinalReadinessGateLaneSurface &lane_b_surface,
    const Objc3FinalReadinessGateLaneSurface &lane_c_surface,
    const Objc3FinalReadinessGateLaneSurface &lane_d_surface);
