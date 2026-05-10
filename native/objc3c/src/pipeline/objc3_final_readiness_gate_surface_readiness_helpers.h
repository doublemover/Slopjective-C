#pragma once

#include "pipeline/objc3_final_readiness_gate_core_feature_implementation_surface.h"

namespace objc3_final_readiness_gate_surface {

void PublishObjc3FinalReadinessGateCoreReadiness(
    Objc3FinalReadinessGateCoreFeatureImplementationSurface &surface,
    const Objc3FrontendLongTailGrammarCoreFeatureSurface &lane_a_surface,
    const Objc3SemanticStabilityCoreFeatureImplementationSurface &lane_b_surface,
    const Objc3LoweringRuntimeStabilityCoreFeatureImplementationSurface &lane_c_surface,
    const Objc3ToolchainRuntimeGaOperationsCoreFeatureExpansionSurface &lane_d_surface);

void PublishObjc3FinalReadinessGateAdvancedShard1Readiness(
    Objc3FinalReadinessGateCoreFeatureImplementationSurface &surface,
    const Objc3FrontendLongTailGrammarCoreFeatureSurface &lane_a_surface,
    const Objc3SemanticStabilityCoreFeatureImplementationSurface &lane_b_surface,
    const Objc3LoweringRuntimeStabilityCoreFeatureImplementationSurface &lane_c_surface,
    const Objc3ToolchainRuntimeGaOperationsCoreFeatureExpansionSurface &lane_d_surface);

void PublishObjc3FinalReadinessGateAdvancedShard3And4Readiness(
    Objc3FinalReadinessGateCoreFeatureImplementationSurface &surface,
    const Objc3FrontendLongTailGrammarCoreFeatureSurface &lane_a_surface,
    const Objc3SemanticStabilityCoreFeatureImplementationSurface &lane_b_surface,
    const Objc3LoweringRuntimeStabilityCoreFeatureImplementationSurface &lane_c_surface,
    const Objc3ToolchainRuntimeGaOperationsCoreFeatureExpansionSurface &lane_d_surface);

void PublishObjc3FinalReadinessGateAdvancedShard2Readiness(
    Objc3FinalReadinessGateCoreFeatureImplementationSurface &surface,
    const Objc3FrontendLongTailGrammarCoreFeatureSurface &lane_a_surface,
    const Objc3SemanticStabilityCoreFeatureImplementationSurface &lane_b_surface,
    const Objc3LoweringRuntimeStabilityCoreFeatureImplementationSurface &lane_c_surface,
    const Objc3ToolchainRuntimeGaOperationsCoreFeatureExpansionSurface &lane_d_surface);

void FinalizeObjc3FinalReadinessGateSurfaceReadiness(
    Objc3FinalReadinessGateCoreFeatureImplementationSurface &surface);

}  // namespace objc3_final_readiness_gate_surface
