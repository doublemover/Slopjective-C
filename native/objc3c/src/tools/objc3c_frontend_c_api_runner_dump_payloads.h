#pragma once

#include <string>
#include <vector>

#include "tools/objc3c_frontend_c_api_runner_dump_publication.h"
#include "tools/objc3c_frontend_c_api_runner_options.h"

std::vector<std::string> BuildFrontendCApiRunnerDumpPayloads(
    const FrontendCApiRunnerOptions &options,
    const FrontendCApiRunnerDumpPublication &publication);
