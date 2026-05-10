#pragma once

#include <iosfwd>

#include "tools/objc3c_frontend_c_api_runner_options.h"
#include "tools/objc3c_frontend_c_api_runner_runtime_inspector_json_context.h"

void WriteFrontendCApiRunnerRuntimeInspectorSections(
    std::ostream &out,
    const FrontendCApiRunnerOptions &options,
    const FrontendCApiRunnerRuntimeInspectorContext &context);
