#pragma once

#include <string>

#include "pipeline/objc3_frontend_types.h"

std::string BuildObjc3LoweringPipelinePassGraphScaffoldKey(
    const Objc3LoweringPipelinePassGraphScaffold &scaffold);

Objc3LoweringPipelinePassGraphScaffold BuildObjc3LoweringPipelinePassGraphScaffold(
    const Objc3FrontendPipelineResult &pipeline_result,
    const Objc3FrontendOptions &options);

bool IsObjc3LoweringPipelinePassGraphScaffoldReady(
    const Objc3LoweringPipelinePassGraphScaffold &scaffold,
    std::string &reason);
