#pragma once

#include <iosfwd>
#include <string>

#include "tools/objc3c_frontend_c_api_runner_diagnostic_totals.h"

void WriteFrontendCApiRunnerObservabilityDiagnosticTotalJsonRows(
    std::ostream &out,
    const std::string &child_indent,
    const FrontendCApiDiagnosticTotals &diagnostic_totals,
    bool result_error_message_present);
