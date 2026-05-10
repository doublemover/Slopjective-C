#pragma once

#include <iosfwd>

#include "tools/objc3c_frontend_c_api_runner_observability_json_context.h"

void WriteFrontendCApiRunnerObservabilitySections(
    std::ostream &out,
    const FrontendCApiRunnerObservabilityContext &context);
