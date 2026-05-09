#pragma once

#include <filesystem>
#include <string>

#include "io/objc3_cli_reporting_output_contract_core_feature_expansion_surface.h"
#include "io/objc3_cli_reporting_output_contract_core_feature_surface.h"
#include "io/objc3_cli_reporting_output_contract_scaffold.h"
#include "libobjc3c_frontend/c_api.h"
#include "tools/objc3c_frontend_c_api_runner_options.h"

struct FrontendCApiRunnerOutputContractCoreSurfaces {
  Objc3CliReportingOutputContractScaffold scaffold;
  Objc3CliReportingOutputContractCoreFeatureSurface core_feature;
  Objc3CliReportingOutputContractCoreFeatureExpansionSurface
      core_feature_expansion;
};

bool BuildFrontendCApiRunnerOutputContractCoreSurfaces(
    const FrontendCApiRunnerOptions &options,
    const std::filesystem::path &summary_path,
    const objc3c_frontend_c_compile_result_t &result,
    FrontendCApiRunnerOutputContractCoreSurfaces &surfaces,
    std::string &error);
