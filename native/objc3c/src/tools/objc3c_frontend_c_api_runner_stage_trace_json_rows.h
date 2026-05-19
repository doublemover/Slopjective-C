#pragma once

#include <ostream>

#include "libobjc3c_frontend/c_api.h"

void WriteFrontendCApiRunnerCompileSessionStageTraceRows(
    std::ostream &out,
    const objc3c_frontend_c_compile_result_t &result);

void WriteFrontendCApiRunnerArtifactStageTraceRows(
    std::ostream &out,
    const objc3c_frontend_c_compile_result_t &result);
