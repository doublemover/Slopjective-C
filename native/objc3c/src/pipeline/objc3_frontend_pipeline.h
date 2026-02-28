#pragma once

#include <string>

#include "pipeline/objc3_frontend_types.h"

Objc3FrontendPipelineResult RunObjc3FrontendPipeline(const std::string &source,
                                                     const Objc3FrontendOptions &options);
