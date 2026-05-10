#pragma once

#include <string>

#include "tools/objc3c_frontend_c_api_runner_output_contract_hardening_surfaces.h"

bool BuildFrontendCApiRunnerOutputContractRecoveryDeterminismSurface(
    FrontendCApiRunnerOutputContractHardeningSurfaces &hardening_surfaces,
    std::string &error);
