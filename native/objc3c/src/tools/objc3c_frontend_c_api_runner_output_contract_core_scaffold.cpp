#include "tools/objc3c_frontend_c_api_runner_output_contract_core_scaffold.h"

#include "tools/objc3c_frontend_c_api_runner_output_contract_failure.h"
#include "tools/objc3c_frontend_c_api_runner_result.h"

bool BuildFrontendCApiRunnerOutputContractScaffoldSurface(
    const FrontendCApiRunnerOptions &options,
    const std::filesystem::path &summary_path,
    const objc3c_frontend_c_compile_result_t &result,
    FrontendCApiRunnerOutputContractCoreSurfaces &surfaces,
    std::string &error) {
  const bool stage_report_output_contract_ready =
      FrontendCApiStageReportShapeReady(result);
  surfaces.scaffold = BuildObjc3CliReportingOutputContractScaffold(
      options.out_dir,
      options.emit_prefix,
      summary_path,
      stage_report_output_contract_ready);
  std::string scaffold_reason;
  if (!IsObjc3CliReportingOutputContractScaffoldReady(surfaces.scaffold,
                                                      scaffold_reason)) {
    return FailFrontendCApiRunnerOutputContract(
        "scaffold", scaffold_reason, error);
  }

  return true;
}
