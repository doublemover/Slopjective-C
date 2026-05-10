#pragma once

#include <ostream>
#include <string>

#include "io/objc3_cli_reporting_output_contract_conformance_matrix_implementation_surface.h"

void WriteFrontendCApiRunnerOutputContractMatrixKeyJsonRows(
    std::ostream &out,
    const std::string &indent,
    const Objc3CliReportingOutputContractConformanceMatrixImplementationSurface
        &matrix);

void WriteFrontendCApiRunnerOutputContractMatrixPathIdentityJsonRows(
    std::ostream &out,
    const std::string &indent,
    const Objc3CliReportingOutputContractConformanceMatrixImplementationSurface
        &matrix);

void WriteFrontendCApiRunnerOutputContractMatrixCoreJsonRows(
    std::ostream &out,
    const std::string &indent,
    const Objc3CliReportingOutputContractConformanceMatrixImplementationSurface
        &matrix);

void WriteFrontendCApiRunnerOutputContractMatrixPathCompatibilityJsonRows(
    std::ostream &out,
    const std::string &indent,
    const Objc3CliReportingOutputContractConformanceMatrixImplementationSurface
        &matrix);

void WriteFrontendCApiRunnerOutputContractMatrixEdgeCaseContractJsonRows(
    std::ostream &out,
    const std::string &indent,
    const Objc3CliReportingOutputContractConformanceMatrixImplementationSurface
        &matrix);

void WriteFrontendCApiRunnerOutputContractMatrixPathBudgetJsonRows(
    std::ostream &out,
    const std::string &indent,
    const Objc3CliReportingOutputContractConformanceMatrixImplementationSurface
        &matrix);

void WriteFrontendCApiRunnerOutputContractMatrixEdgeCaseRobustnessJsonRows(
    std::ostream &out,
    const std::string &indent,
    const Objc3CliReportingOutputContractConformanceMatrixImplementationSurface
        &matrix);

void WriteFrontendCApiRunnerOutputContractMatrixHardeningJsonRows(
    std::ostream &out,
    const std::string &indent,
    const Objc3CliReportingOutputContractConformanceMatrixImplementationSurface
        &matrix);

void WriteFrontendCApiRunnerOutputContractMatrixReadinessJsonRows(
    std::ostream &out,
    const std::string &indent,
    const Objc3CliReportingOutputContractConformanceMatrixImplementationSurface
        &matrix);
