#pragma once

#include <string>

#include "libobjc3c_frontend/c_api.h"
#include "tools/objc3c_frontend_c_api_runner_c_string.h"
#include "tools/objc3c_frontend_c_api_runner_public_result_ownership.h"

struct FrontendCApiRunnerResultArtifactPathProbe {
  FrontendCApiRunnerStringSnapshot path;
};

FrontendCApiRunnerResultArtifactPathProbe
ProbeFrontendCApiResultOwnedArtifactPath(
    const objc3c_frontend_c_compile_result_t &result,
    const FrontendCApiRunnerCArtifactRequirement &requirement);

bool ProbeFrontendCApiResultOwnedArtifactAvailability(
    const objc3c_frontend_c_compile_result_t &result,
    objc3c_frontend_c_artifact_kind_t artifact_kind);

bool ValidateFrontendCApiResultOwnedArtifactRequirementContract(
    const FrontendCApiRunnerStringSnapshot &path,
    bool has_artifact,
    const FrontendCApiRunnerCArtifactRequirement &requirement,
    std::string &reason);
