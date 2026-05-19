#pragma once

#include <string>

#include "tools/objc3c_frontend_c_api_runner_compile_session.h"
#include "tools/objc3c_frontend_c_api_runner_options.h"

bool RunFrontendCApiRunnerCompileSession(
    const FrontendCApiRunnerOptions &options,
    FrontendCApiRunnerCompileSession &session,
    std::string &error);
