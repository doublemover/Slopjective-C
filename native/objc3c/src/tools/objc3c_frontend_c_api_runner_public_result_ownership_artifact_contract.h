#pragma once

#include "tools/objc3c_frontend_c_api_runner_public_result_ownership.h"

FrontendCApiRunnerCArtifactOwnershipContract
BuildFrontendCApiRunnerCArtifactOwnershipContract(
    const objc3c_frontend_c_compile_result_t &result,
    const FrontendCApiRunnerCArtifactRequirement &requirement);
