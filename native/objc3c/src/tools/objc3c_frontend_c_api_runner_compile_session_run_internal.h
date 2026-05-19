#pragma once

#include <string>

#include "tools/objc3c_frontend_c_api_runner_compile_session.h"
#include "tools/objc3c_frontend_c_api_runner_invocation.h"
#include "tools/objc3c_frontend_c_api_runner_options.h"

bool ExecuteFrontendCApiRunnerCompileContext(
    const FrontendCApiRunnerOptions &options,
    FrontendCApiRunnerCompileSession &session,
    FrontendCApiContextOwner &context,
    std::string &error);

void CaptureFrontendCApiRunnerCompileResultSnapshots(
    const FrontendCApiContextOwner &context,
    FrontendCApiRunnerCompileSession &session);

bool PublishFrontendCApiRunnerCompileSessionResult(
    const FrontendCApiRunnerOptions &options,
    FrontendCApiRunnerCompileSession &session,
    std::string &error);
