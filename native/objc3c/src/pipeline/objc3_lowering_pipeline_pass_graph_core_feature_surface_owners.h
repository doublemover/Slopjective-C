#pragma once

#include "pipeline/objc3_lowering_pipeline_pass_graph_core_feature_surface.h"

namespace objc3_lowering_pipeline_pass_graph_core_feature {

void PopulateEvidence(
    Objc3LoweringPipelinePassGraphCoreFeatureSurface &surface,
    const Objc3FrontendPipelineResult &pipeline_result);

void PublishReadiness(
    Objc3LoweringPipelinePassGraphCoreFeatureSurface &surface,
    const Objc3FrontendPipelineResult &pipeline_result,
    const Objc3FrontendOptions &options);

void PublishFailureReason(
    Objc3LoweringPipelinePassGraphCoreFeatureSurface &surface);

}  // namespace objc3_lowering_pipeline_pass_graph_core_feature
