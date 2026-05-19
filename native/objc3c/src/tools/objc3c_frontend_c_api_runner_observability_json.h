#pragma once

#include <ostream>
#include <string>

#include "tools/objc3c_frontend_c_api_runner_observability_publication.h"

void WriteFrontendCApiRunnerObservabilityJson(
    std::ostream &out,
    const std::string &indent,
    const FrontendCApiRunnerObservabilityPublication &publication);

std::string BuildFrontendCApiRunnerObservabilityJson(
    const FrontendCApiRunnerObservabilityPublication &publication);
