#pragma once

#include <string>

#include "tools/objc3c_frontend_c_api_runner_output_contract_hardening_surfaces.h"

bool BuildFrontendCApiRunnerOutputContractEdgeCaseSetup(
    const FrontendCApiRunnerOutputContractCoreSurfaces &core_surfaces,
    FrontendCApiRunnerOutputContractHardeningSurfaces &hardening_surfaces,
    std::string &error);

bool BuildFrontendCApiRunnerOutputContractEdgeCaseHardeningExpectations(
    FrontendCApiRunnerOutputContractHardeningSurfaces &hardening_surfaces,
    std::string &error);
