#pragma once

#include <iosfwd>

#include "tools/objc3c_frontend_c_api_runner_public_result.h"

void WriteFrontendCApiRunnerPublicResultDiagnosticJsonRows(
    std::ostream &out,
    const FrontendCApiRunnerPublicResultView &public_result);
