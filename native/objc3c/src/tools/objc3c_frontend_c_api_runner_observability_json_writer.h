#pragma once

#include <iosfwd>
#include <string>

#include "libobjc3c_frontend/c_api.h"
#include "tools/objc3c_frontend_c_api_runner_observability_json_context.h"

FrontendCApiRunnerObservabilityContext
BuildFrontendCApiRunnerObservabilityWriterContext(
    const std::string &indent,
    const FrontendCApiRunnerObservabilityPublication &publication);

void WriteFrontendCApiRunnerObservabilityJsonSectionRows(
    std::ostream &out,
    const FrontendCApiRunnerObservabilityContext &context);
