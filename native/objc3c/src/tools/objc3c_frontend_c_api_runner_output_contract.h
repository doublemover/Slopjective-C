#pragma once

#include <filesystem>
#include <string>

#include "io/objc3_cli_reporting_output_contract_conformance_corpus_expansion_surface.h"
#include "io/objc3_cli_reporting_output_contract_conformance_matrix_implementation_surface.h"
#include "libobjc3c_frontend/c_api.h"
#include "tools/objc3c_frontend_c_api_runner_options.h"

struct FrontendCApiRunnerOutputContract {
  Objc3CliReportingOutputContractConformanceMatrixImplementationSurface
      conformance_matrix_surface;
  Objc3CliReportingOutputContractConformanceCorpusExpansionSurface
      conformance_corpus_surface;
};

bool BuildFrontendCApiRunnerOutputContract(
    const FrontendCApiRunnerOptions &options,
    const std::filesystem::path &summary_path,
    const objc3c_frontend_c_compile_result_t &result,
    FrontendCApiRunnerOutputContract &output_contract,
    std::string &error);
