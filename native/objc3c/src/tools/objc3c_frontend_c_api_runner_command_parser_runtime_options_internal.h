#pragma once

#include <string>

#include "tools/objc3c_frontend_c_api_runner_command_parser_options.h"

FrontendCApiRunnerCommandOptionParseResult
ParseFrontendCApiRunnerMaxMessageArgsOption(
    const std::string &arg,
    int argc,
    char **argv,
    int &index,
    FrontendCApiRunnerOptions &options,
    std::string &error);

FrontendCApiRunnerCommandOptionParseResult
ParseFrontendCApiRunnerRuntimeDispatchSymbolOption(
    const std::string &arg,
    int argc,
    char **argv,
    int &index,
    FrontendCApiRunnerOptions &options,
    std::string &error);

FrontendCApiRunnerCommandOptionParseResult
ParseFrontendCApiRunnerRegistrationOrdinalOption(
    const std::string &arg,
    int argc,
    char **argv,
    int &index,
    FrontendCApiRunnerOptions &options,
    std::string &error);
