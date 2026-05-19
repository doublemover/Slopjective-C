#pragma once

#include <filesystem>
#include <string>

#include "libobjc3c_frontend/c_api.h"
#include "tools/objc3c_frontend_c_api_runner_output_contract_core.h"
#include "tools/objc3c_frontend_c_api_runner_options.h"

bool BuildFrontendCApiRunnerOutputContractCoreFeatureSurface(
    const FrontendCApiRunnerOptions &options,
    const std::filesystem::path &summary_path,
    const objc3c_frontend_c_compile_result_t &result,
    FrontendCApiRunnerOutputContractCoreSurfaces &surfaces,
    std::filesystem::path &diagnostics_output_path,
    std::string &error);
