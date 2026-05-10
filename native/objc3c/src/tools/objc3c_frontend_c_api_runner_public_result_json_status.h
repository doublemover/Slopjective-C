#pragma once

#include <iosfwd>

#include "tools/objc3c_frontend_c_api_runner_options.h"
#include "tools/objc3c_frontend_c_api_runner_public_result.h"

void WriteFrontendCApiRunnerPublicResultStatusJsonRows(
    std::ostream &out,
    const FrontendCApiRunnerOptions &options,
    const FrontendCApiRunnerPublicResultView &public_result);
