#pragma once

#include <iosfwd>

#include "libobjc3c_frontend/c_api.h"
#include "tools/objc3c_frontend_c_api_runner_options.h"
#include "tools/objc3c_frontend_c_api_runner_playground_repro_json_context.h"

void WriteFrontendCApiRunnerPlaygroundReproContractSourceProfileSections(
    std::ostream &out,
    const FrontendCApiRunnerPlaygroundReproContext &context,
    const FrontendCApiRunnerOptions &options,
    const objc3c_frontend_c_compile_result_t &result);

void WriteFrontendCApiRunnerPlaygroundReproPublicSurfaceSection(
    std::ostream &out,
    const FrontendCApiRunnerPlaygroundReproContext &context);

void WriteFrontendCApiRunnerPlaygroundReproDumpCommandSection(
    std::ostream &out,
    const FrontendCApiRunnerPlaygroundReproContext &context,
    const FrontendCApiRunnerOptions &options);
