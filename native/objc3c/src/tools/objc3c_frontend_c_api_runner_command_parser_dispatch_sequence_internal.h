#pragma once

#include "tools/objc3c_frontend_c_api_runner_command_parser_dispatch_internal.h"

FrontendCApiRunnerCommandOptionParseResult
DispatchFrontendCApiRunnerCommandOptionPathRuntimePass(
    const std::string &arg,
    int argc,
    char **argv,
    int &index,
    FrontendCApiRunnerOptions &options,
    std::string &error);

FrontendCApiRunnerCommandOptionParseResult
DispatchFrontendCApiRunnerCommandOptionBackendPass(
    const std::string &arg,
    int argc,
    char **argv,
    int &index,
    FrontendCApiRunnerOptions &options,
    std::string &error);

FrontendCApiRunnerCommandOptionParseResult
DispatchFrontendCApiRunnerCommandOptionEmissionHelpPass(
    const std::string &arg,
    FrontendCApiRunnerOptions &options,
    std::string &error);
