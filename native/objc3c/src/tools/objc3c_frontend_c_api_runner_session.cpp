#include "tools/objc3c_frontend_c_api_runner_session.h"

#include <filesystem>
#include <iostream>
#include <string>

#include "libobjc3c_frontend/c_api.h"
#include "tools/objc3c_frontend_c_api_runner_dump_actions.h"
#include "tools/objc3c_frontend_c_api_runner_invocation.h"
#include "tools/objc3c_frontend_c_api_runner_output_contract.h"
#include "tools/objc3c_frontend_c_api_runner_result.h"
#include "tools/objc3c_frontend_c_api_runner_summary_io.h"
#include "tools/objc3c_frontend_c_api_runner_summary_json.h"

namespace fs = std::filesystem;

int RunFrontendCApiRunnerSession(const FrontendCApiRunnerOptions &options) {
  FrontendCApiContextOwner context;
  if (!context.valid()) {
    std::cerr << "failed to allocate frontend context\n";
    return 2;
  }

  const FrontendCApiRunnerCompileInvocation compile_invocation(options);
  objc3c_frontend_c_compile_result_t result = {};
  const FrontendCApiCompileResultGuard result_guard{&result};
  const objc3c_frontend_c_status_t status =
      objc3c_frontend_c_compile_file(
          context.get(),
          compile_invocation.compile_options(),
          &result);
  const std::string last_error = ReadFrontendCApiLastError(context.get());
  const std::string result_error_message =
      FrontendCApiResultErrorMessage(result);
  std::string accessor_contract_error;
  if (!ValidateFrontendCApiResultAccessors(
          status,
          result,
          last_error,
          result_error_message,
          accessor_contract_error)) {
    std::cerr << "frontend C API accessor contract fail-closed: "
              << accessor_contract_error << "\n";
    return 2;
  }
  const int exit_code = FrontendCApiExitCodeFromStatus(status, result);

  const fs::path summary_path =
      options.summary_out.empty()
          ? (options.out_dir / (options.emit_prefix + ".c_api_summary.json"))
          : options.summary_out;
  FrontendCApiRunnerOutputContract output_contract;
  std::string output_contract_error;
  if (!BuildFrontendCApiRunnerOutputContract(
          options,
          summary_path,
          result,
          output_contract,
          output_contract_error)) {
    std::cerr << output_contract_error << "\n";
    return 2;
  }

  const std::string runtime_metadata_binary_path_text =
      FrontendCApiResultArtifactPath(
          result,
          OBJC3C_FRONTEND_ARTIFACT_RUNTIME_METADATA);
  const std::string summary_json = BuildFrontendCApiRunnerSummaryJson(
      options,
      summary_path,
      status,
      result,
      last_error,
      result_error_message,
      output_contract);
  std::string summary_error;
  if (!WriteFrontendCApiRunnerSummaryFile(summary_path,
                                          summary_json,
                                          summary_error)) {
    std::cerr << summary_error << "\n";
    return 2;
  }

  if (ShouldEmitFrontendCApiRunnerDumpActions(options)) {
    EmitFrontendCApiRunnerDumpActions(
        options,
        summary_path,
        result,
        status,
        result_error_message,
        runtime_metadata_binary_path_text,
        summary_json);
  } else {
    std::cout << "wrote summary: " << summary_path.generic_string() << "\n";
  }

  if (!last_error.empty()) {
    std::cerr << last_error << "\n";
  }
  return exit_code;
}
