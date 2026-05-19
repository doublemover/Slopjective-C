#pragma once

#include <string>

#include "libobjc3c_frontend/c_api.h"
#include "tools/objc3c_frontend_c_api_runner_options.h"

void ApplyFrontendCApiRunnerCompilePathInputOptions(
    objc3c_frontend_c_compile_options_t &compile_options,
    const FrontendCApiRunnerOptions &runner_options,
    const std::string &input_path_text,
    const std::string &out_dir_text);

void ApplyFrontendCApiRunnerCompileBackendToolchainOptions(
    objc3c_frontend_c_compile_options_t &compile_options,
    const FrontendCApiRunnerOptions &runner_options,
    const std::string &clang_path_text,
    const std::string &llc_path_text);

void ApplyFrontendCApiRunnerCompileRuntimeOptions(
    objc3c_frontend_c_compile_options_t &compile_options,
    const FrontendCApiRunnerOptions &runner_options);

void ApplyFrontendCApiRunnerCompileEmissionOptions(
    objc3c_frontend_c_compile_options_t &compile_options,
    const FrontendCApiRunnerOptions &runner_options);
