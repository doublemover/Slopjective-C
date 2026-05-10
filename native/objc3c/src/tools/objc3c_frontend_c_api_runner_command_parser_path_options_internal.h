#pragma once

#include <string>

#include "tools/objc3c_frontend_c_api_runner_command_parser_options.h"

FrontendCApiRunnerCommandOptionParseResult
ParseFrontendCApiRunnerOutputPathStringOption(
    const std::string &arg,
    int argc,
    char **argv,
    int &index,
    FrontendCApiRunnerOptions &options);

FrontendCApiRunnerCommandOptionParseResult
ParseFrontendCApiRunnerToolchainPathStringOption(
    const std::string &arg,
    int argc,
    char **argv,
    int &index,
    FrontendCApiRunnerOptions &options);

FrontendCApiRunnerCommandOptionParseResult
ParseFrontendCApiRunnerSummaryOutputPathStringOption(
    const std::string &arg,
    int argc,
    char **argv,
    int &index,
    FrontendCApiRunnerOptions &options);
