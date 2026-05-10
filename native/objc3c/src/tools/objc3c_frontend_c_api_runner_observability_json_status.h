#pragma once

#include <iosfwd>
#include <string>

#include "libobjc3c_frontend/c_api.h"

void WriteFrontendCApiRunnerObservabilityStatusStageJsonRows(
    std::ostream &out,
    const std::string &child_indent,
    objc3c_frontend_c_status_t status,
    const std::string &last_attempted_stage,
    const std::string &blocking_stage);
