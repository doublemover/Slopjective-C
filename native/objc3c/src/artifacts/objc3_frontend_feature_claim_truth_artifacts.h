#pragma once

#include <string>

#include "pipeline/objc3_frontend_types.h"

namespace objc3::artifacts::frontend {

std::string BuildFeatureClaimStrictnessTruthSurfaceReplayKey(
    const Objc3FrontendOptions &options,
    const Objc3FrontendPipelineResult &pipeline_result);

std::string BuildFeatureClaimStrictnessTruthSurfaceJson(
    const Objc3FrontendOptions &options,
    const Objc3FrontendPipelineResult &pipeline_result);

}  // namespace objc3::artifacts::frontend
