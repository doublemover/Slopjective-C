#pragma once

#include <string>

#include "tools/objc3c_frontend_c_api_runner_observability_publication.h"

struct FrontendCApiRunnerObservabilityContext {
  FrontendCApiRunnerObservabilityPublication publication;
  std::string child_indent;
  std::string grandchild_indent;
};

FrontendCApiRunnerObservabilityContext
BuildFrontendCApiRunnerObservabilityContext(
    const std::string &indent,
    const FrontendCApiRunnerObservabilityPublication &publication);
