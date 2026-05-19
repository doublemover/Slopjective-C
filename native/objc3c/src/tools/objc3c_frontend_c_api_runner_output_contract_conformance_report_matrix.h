#pragma once

#include <string>

#include "io/objc3_cli_reporting_output_contract_conformance_matrix_implementation_surface.h"
#include "tools/objc3c_frontend_c_api_runner_output_contract_hardening_surfaces.h"

bool BuildFrontendCApiRunnerOutputContractConformanceMatrix(
    const FrontendCApiRunnerOutputContractHardeningSurfaces &hardening_surfaces,
    Objc3CliReportingOutputContractConformanceMatrixImplementationSurface
        &conformance_matrix,
    std::string &error);
