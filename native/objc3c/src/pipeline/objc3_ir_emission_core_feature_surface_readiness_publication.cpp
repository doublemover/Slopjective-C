#include "pipeline/objc3_ir_emission_core_feature_implementation_surface.h"
#include "pipeline/objc3_ir_emission_core_feature_surface_readiness_publication.h"

namespace objc3_ir_emission_core_feature_surface {

void PublishObjc3IREmissionCoreFeatureSurfaceReadiness(
    Objc3IREmissionCoreFeatureImplementationSurface &surface,
    const Objc3FrontendPipelineResult &pipeline_result) {
  const Objc3IREmissionCompletenessScaffold &scaffold =
      pipeline_result.ir_emission_completeness_scaffold;
  const Objc3ParseLoweringReadinessSurface &parse_surface =
      pipeline_result.parse_lowering_readiness_surface;
  const Objc3TypedSemaToLoweringContractSurface &typed_surface =
      pipeline_result.typed_sema_to_lowering_contract_surface;

  PublishObjc3IREmissionCoreFeatureBaseReadiness(surface, scaffold);
  PublishObjc3IREmissionCoreFeatureQualityReadiness(surface, parse_surface);
  PublishObjc3IREmissionCoreFeatureAdvancedShard1Readiness(surface,
                                                           parse_surface,
                                                           typed_surface);
}

}  // namespace objc3_ir_emission_core_feature_surface
