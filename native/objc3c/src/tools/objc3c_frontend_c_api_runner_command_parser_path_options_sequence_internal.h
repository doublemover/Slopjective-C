#pragma once

#include "tools/objc3c_frontend_c_api_runner_command_parser_path_options_internal.h"

FrontendCApiRunnerCommandOptionParseResult
ParseFrontendCApiRunnerPathStringOutputPass(
    const std::string &arg,
    int argc,
    char **argv,
    int &index,
    FrontendCApiRunnerOptions &options);

FrontendCApiRunnerCommandOptionParseResult
ParseFrontendCApiRunnerPathStringToolchainPass(
    const std::string &arg,
    int argc,
    char **argv,
    int &index,
    FrontendCApiRunnerOptions &options);

FrontendCApiRunnerCommandOptionParseResult
ParseFrontendCApiRunnerPathStringSummaryOutputPass(
    const std::string &arg,
    int argc,
    char **argv,
    int &index,
    FrontendCApiRunnerOptions &options);
