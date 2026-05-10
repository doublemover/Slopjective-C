#pragma once

#include <string>

#include "tools/objc3c_frontend_c_api_runner_command_parser_options.h"

bool FrontendCApiRunnerDispatchShouldReturn(
    FrontendCApiRunnerCommandOptionParseResult result);

FrontendCApiRunnerCommandOptionParseResult
DispatchFrontendCApiRunnerCommandOptionPassSequence(
    const std::string &arg,
    int argc,
    char **argv,
    int &index,
    FrontendCApiRunnerOptions &options,
    std::string &error);
