#include "pipeline/objc3_final_readiness_gate_surface_owners.h"

Objc3FinalReadinessGateCoreFeatureImplementationSurface
BuildObjc3FinalReadinessGateCoreFeatureImplementationSurface(
    const Objc3FinalReadinessGateCoreFeatureScaffold &scaffold,
    const Objc3FrontendLongTailGrammarCoreFeatureSurface &lane_a_surface,
    const Objc3SemanticStabilityCoreFeatureImplementationSurface &lane_b_surface,
    const Objc3LoweringRuntimeStabilityCoreFeatureImplementationSurface &lane_c_surface,
    const Objc3ToolchainRuntimeGaOperationsCoreFeatureExpansionSurface &lane_d_surface) {
  Objc3FinalReadinessGateCoreFeatureImplementationSurface surface;
  objc3_final_readiness_gate_surface::
      PopulateObjc3FinalReadinessGateSurfaceEvidence(surface,
                                                     scaffold,
                                                     lane_a_surface,
                                                     lane_b_surface,
                                                     lane_c_surface,
                                                     lane_d_surface);
  objc3_final_readiness_gate_surface::
      PublishObjc3FinalReadinessGateSurfaceReadiness(surface,
                                                     lane_a_surface,
                                                     lane_b_surface,
                                                     lane_c_surface,
                                                     lane_d_surface);
  objc3_final_readiness_gate_surface::
      PublishObjc3FinalReadinessGateSurfaceFailureReasons(surface,
                                                          lane_a_surface,
                                                          lane_b_surface,
                                                          lane_c_surface,
                                                          lane_d_surface);
  return surface;
}
