#include "pipeline/objc3_final_readiness_gate_surface_readiness_helpers.h"

namespace objc3_final_readiness_gate_surface {

void PublishObjc3FinalReadinessGateSurfaceReadiness(
    Objc3FinalReadinessGateCoreFeatureImplementationSurface &surface,
    const Objc3FrontendLongTailGrammarCoreFeatureSurface &lane_a_surface,
    const Objc3SemanticStabilityCoreFeatureImplementationSurface &lane_b_surface,
    const Objc3LoweringRuntimeStabilityCoreFeatureImplementationSurface &lane_c_surface,
    const Objc3ToolchainRuntimeGaOperationsCoreFeatureExpansionSurface &lane_d_surface) {
  PublishObjc3FinalReadinessGateCoreReadiness(
      surface, lane_a_surface, lane_b_surface, lane_c_surface, lane_d_surface);
  PublishObjc3FinalReadinessGateAdvancedShard1Readiness(
      surface, lane_a_surface, lane_b_surface, lane_c_surface, lane_d_surface);
  PublishObjc3FinalReadinessGateAdvancedShard3And4Readiness(
      surface, lane_a_surface, lane_b_surface, lane_c_surface, lane_d_surface);
  PublishObjc3FinalReadinessGateAdvancedShard2Readiness(
      surface, lane_a_surface, lane_b_surface, lane_c_surface, lane_d_surface);
  FinalizeObjc3FinalReadinessGateSurfaceReadiness(surface);
}

}  // namespace objc3_final_readiness_gate_surface
