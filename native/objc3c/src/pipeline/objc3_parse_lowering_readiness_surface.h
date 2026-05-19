#pragma once

#include <string>

#include "pipeline/objc3_frontend_types.h"

Objc3ParseLoweringReadinessSurface BuildObjc3ParseLoweringReadinessSurface(
    const Objc3FrontendPipelineResult &pipeline_result,
    const Objc3FrontendOptions &options);

bool IsObjc3ParseLoweringReadinessSurfaceReady(
    const Objc3ParseLoweringReadinessSurface &surface,
    std::string &reason);
