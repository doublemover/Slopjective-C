#pragma once

#include <filesystem>
#include <ostream>

#include "tools/objc3c_frontend_c_api_runner_options.h"

void AppendFrontendCApiRunnerReproBaseInvocationAndPathArgs(
    std::ostream &command,
    const FrontendCApiRunnerOptions &options);

void AppendFrontendCApiRunnerReproSummaryPathArg(
    std::ostream &command,
    const std::filesystem::path &summary_path);

void AppendFrontendCApiRunnerReproToolchainArgs(
    std::ostream &command,
    const FrontendCApiRunnerOptions &options);

void AppendFrontendCApiRunnerReproBackendArg(
    std::ostream &command,
    const FrontendCApiRunnerOptions &options);

void AppendFrontendCApiRunnerReproRuntimeArgs(
    std::ostream &command,
    const FrontendCApiRunnerOptions &options);

void AppendFrontendCApiRunnerReproEmissionDumpFlags(
    std::ostream &command,
    const FrontendCApiRunnerOptions &options,
    bool dump_playground_repro_json);
