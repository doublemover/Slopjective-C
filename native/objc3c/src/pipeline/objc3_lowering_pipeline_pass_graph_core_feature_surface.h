#pragma once

#include <string>

#include "pipeline/objc3_frontend_types.h"

std::string BuildObjc3LoweringPipelinePassGraphCoreFeatureKey(
    const Objc3LoweringPipelinePassGraphCoreFeatureSurface &surface);

Objc3LoweringPipelinePassGraphCoreFeatureSurface
BuildObjc3LoweringPipelinePassGraphCoreFeatureSurface(
    const Objc3FrontendPipelineResult &pipeline_result,
    const Objc3FrontendOptions &options);

bool IsObjc3LoweringPipelinePassGraphCoreFeatureSurfaceReady(
    const Objc3LoweringPipelinePassGraphCoreFeatureSurface &surface,
    std::string &reason);
