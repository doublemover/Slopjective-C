#include "pipeline/objc3_lowering_pipeline_pass_graph_core_feature_surface_owners.h"

Objc3LoweringPipelinePassGraphCoreFeatureSurface
BuildObjc3LoweringPipelinePassGraphCoreFeatureSurface(
    const Objc3FrontendPipelineResult &pipeline_result,
    const Objc3FrontendOptions &options) {
  Objc3LoweringPipelinePassGraphCoreFeatureSurface surface;
  objc3_lowering_pipeline_pass_graph_core_feature::PopulateEvidence(
      surface, pipeline_result);
  objc3_lowering_pipeline_pass_graph_core_feature::PublishReadiness(
      surface, pipeline_result, options);
  objc3_lowering_pipeline_pass_graph_core_feature::PublishFailureReason(surface);
  return surface;
}
