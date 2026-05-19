#include "pipeline/objc3_ir_emission_core_feature_implementation_surface.h"

namespace objc3_ir_emission_core_feature_surface {

void PopulateObjc3IREmissionCoreFeatureSurfaceEvidence(
    Objc3IREmissionCoreFeatureImplementationSurface &surface,
    const Objc3FrontendPipelineResult &pipeline_result);

void PublishObjc3IREmissionCoreFeatureSurfaceReadiness(
    Objc3IREmissionCoreFeatureImplementationSurface &surface,
    const Objc3FrontendPipelineResult &pipeline_result);

void PublishObjc3IREmissionCoreFeatureSurfaceFailureReasons(
    Objc3IREmissionCoreFeatureImplementationSurface &surface);

}  // namespace objc3_ir_emission_core_feature_surface

Objc3IREmissionCoreFeatureImplementationSurface
BuildObjc3IREmissionCoreFeatureImplementationSurface(
    const Objc3FrontendPipelineResult &pipeline_result) {
  Objc3IREmissionCoreFeatureImplementationSurface surface;
  objc3_ir_emission_core_feature_surface::
      PopulateObjc3IREmissionCoreFeatureSurfaceEvidence(surface,
                                                        pipeline_result);
  objc3_ir_emission_core_feature_surface::
      PublishObjc3IREmissionCoreFeatureSurfaceReadiness(surface,
                                                        pipeline_result);
  objc3_ir_emission_core_feature_surface::
      PublishObjc3IREmissionCoreFeatureSurfaceFailureReasons(surface);
  return surface;
}
