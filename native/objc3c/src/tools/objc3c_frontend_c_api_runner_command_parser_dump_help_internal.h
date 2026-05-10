#pragma once

#include <string>

#include "tools/objc3c_frontend_c_api_runner_command_parser_options.h"

FrontendCApiRunnerCommandOptionParseResult
ParseFrontendCApiRunnerEmissionToggleOption(
    const std::string &arg,
    FrontendCApiRunnerOptions &options);

FrontendCApiRunnerCommandOptionParseResult
ParseFrontendCApiRunnerDumpFlagOption(const std::string &arg,
                                      FrontendCApiRunnerOptions &options);

FrontendCApiRunnerCommandOptionParseResult
ParseFrontendCApiRunnerHelpOption(const std::string &arg,
                                  std::string &error);
