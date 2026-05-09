#pragma once

#include <filesystem>
#include <string>

#include "tools/objc3c_frontend_c_api_runner_artifact_paths.h"
#include "tools/objc3c_frontend_c_api_runner_compile_session.h"
#include "tools/objc3c_frontend_c_api_runner_options.h"

struct FrontendCApiRunnerSessionSummary {
  FrontendCApiRunnerArtifactPathView artifact_paths;
  std::string json;
};

bool BuildFrontendCApiRunnerSessionSummary(
    const FrontendCApiRunnerOptions &options,
    const std::filesystem::path &summary_path,
    const FrontendCApiRunnerCompileSession &compile_session,
    FrontendCApiRunnerSessionSummary &summary,
    std::string &error);
