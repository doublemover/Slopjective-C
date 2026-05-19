#pragma once

#include <string>

#include "tools/objc3c_frontend_c_api_runner_output_contract.h"
#include "tools/objc3c_frontend_c_api_runner_output_contract_core.h"

bool BuildFrontendCApiRunnerOutputContractHardeningSurfaces(
    const FrontendCApiRunnerOutputContractCoreSurfaces &core_surfaces,
    FrontendCApiRunnerOutputContract &output_contract,
    std::string &error);
