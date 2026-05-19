#pragma once

#include <filesystem>
#include <string>

#include "libobjc3c_frontend/c_api.h"
#include "tools/objc3c_frontend_c_api_runner_artifact_paths.h"
#include "tools/objc3c_frontend_c_api_runner_compile_session.h"
#include "tools/objc3c_frontend_c_api_runner_options.h"
#include "tools/objc3c_frontend_c_api_runner_output_contract.h"
#include "tools/objc3c_frontend_c_api_runner_public_result.h"

struct FrontendCApiRunnerSessionResult {
  const objc3c_frontend_c_compile_result_t *compile_result = nullptr;
  objc3c_frontend_c_status_t status = OBJC3C_FRONTEND_STATUS_INTERNAL_ERROR;
  FrontendCApiRunnerArtifactPathView artifact_paths;
  FrontendCApiRunnerOutputContract output_contract;
  FrontendCApiRunnerPublicResultView public_result;
  std::string json;
};

bool BuildFrontendCApiRunnerSessionResult(
    const FrontendCApiRunnerOptions &options,
    const std::filesystem::path &summary_path,
    const FrontendCApiRunnerCompileSession &compile_session,
    FrontendCApiRunnerSessionResult &session_result,
    std::string &error);
