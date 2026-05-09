#pragma once

#include <string>

#include "libobjc3c_frontend/c_api.h"

struct FrontendCApiRunnerStringSnapshot {
  bool present = false;
  std::string text;
};

FrontendCApiRunnerStringSnapshot SnapshotOptionalFrontendCApiString(
    const objc3c_frontend_c_string_t *value);
std::string OptionalFrontendCApiString(
    const objc3c_frontend_c_string_t *value);
FrontendCApiRunnerStringSnapshot FrontendCApiResultArtifactPathSnapshot(
    const objc3c_frontend_c_compile_result_t &result,
    objc3c_frontend_c_artifact_kind_t artifact_kind);
std::string FrontendCApiResultArtifactPath(
    const objc3c_frontend_c_compile_result_t &result,
    objc3c_frontend_c_artifact_kind_t artifact_kind);
FrontendCApiRunnerStringSnapshot FrontendCApiResultErrorMessageSnapshot(
    const objc3c_frontend_c_compile_result_t &result);
std::string FrontendCApiResultErrorMessage(
    const objc3c_frontend_c_compile_result_t &result);
std::string ReadFrontendCApiLastError(
    const objc3c_frontend_c_context_t *context);
