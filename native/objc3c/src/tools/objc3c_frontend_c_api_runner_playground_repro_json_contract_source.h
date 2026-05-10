#pragma once

#include <iosfwd>
#include <string>

#include "libobjc3c_frontend/c_api.h"
#include "tools/objc3c_frontend_c_api_runner_artifact_paths.h"
#include "tools/objc3c_frontend_c_api_runner_options.h"

void WriteFrontendCApiRunnerPlaygroundReproContractSourceJsonRows(
    std::ostream &out,
    const std::string &child_indent,
    const FrontendCApiRunnerOptions &options,
    const objc3c_frontend_c_compile_result_t &result,
    const FrontendCApiRunnerArtifactPathView &paths);
