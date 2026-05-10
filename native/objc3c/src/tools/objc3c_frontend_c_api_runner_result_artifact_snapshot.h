#pragma once

#include <string>

#include "libobjc3c_frontend/c_api.h"
#include "tools/objc3c_frontend_c_api_runner_string_snapshot.h"

struct FrontendCApiRunnerResultArtifactSnapshot {
  bool produced = false;
  FrontendCApiRunnerStringSnapshot path;
};

FrontendCApiRunnerResultArtifactSnapshot
CaptureFrontendCApiRunnerResultArtifactSnapshot(
    const objc3c_frontend_c_compile_result_t &result,
    objc3c_frontend_c_artifact_kind_t artifact_kind);
std::string FrontendCApiRunnerResultArtifactPathText(
    const objc3c_frontend_c_compile_result_t &result,
    objc3c_frontend_c_artifact_kind_t artifact_kind);
