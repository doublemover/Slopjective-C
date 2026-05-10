#pragma once

#include <filesystem>
#include <string>

#include "tools/objc3c_frontend_c_api_runner_output_contract_core.h"
#include "tools/objc3c_frontend_c_api_runner_options.h"

bool BuildFrontendCApiRunnerOutputContractCoreFeatureExpansionSurface(
    const FrontendCApiRunnerOptions &options,
    const std::filesystem::path &summary_path,
    const std::filesystem::path &diagnostics_output_path,
    FrontendCApiRunnerOutputContractCoreSurfaces &surfaces,
    std::string &error);
