#pragma once

#include "pipeline/objc3_frontend_types.h"

void BuildObjc3ParseLoweringParserBehaviorReadiness(
    Objc3ParseLoweringReadinessSurface &surface,
    const Objc3FrontendPipelineResult &pipeline_result,
    const Objc3FrontendOptions &options);
