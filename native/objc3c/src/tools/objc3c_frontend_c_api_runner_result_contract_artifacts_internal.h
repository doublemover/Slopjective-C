#pragma once

#include <string>

#include "libobjc3c_frontend/c_api.h"
#include "tools/objc3c_frontend_c_api_runner_public_result_ownership.h"
#include "tools/objc3c_frontend_c_api_runner_result_artifact_snapshot.h"

bool ValidateFrontendCApiResultOwnedArtifactRequirementContract(
    const FrontendCApiRunnerResultArtifactSnapshot &snapshot,
    const FrontendCApiRunnerCArtifactRequirement &requirement,
    std::string &reason);
