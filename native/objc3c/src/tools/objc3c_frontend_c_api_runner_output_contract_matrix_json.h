#pragma once

#include <ostream>
#include <string>

#include "io/objc3_cli_reporting_output_contract_conformance_matrix_implementation_surface.h"

void WriteFrontendCApiRunnerOutputContractMatrixJson(
    std::ostream &out,
    const std::string &indent,
    const Objc3CliReportingOutputContractConformanceMatrixImplementationSurface
        &matrix);
