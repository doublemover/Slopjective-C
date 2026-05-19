#pragma once

#include <string>

#include "tools/objc3c_frontend_c_api_runner_command_parser_options.h"

bool FrontendCApiRunnerOptionParseFailed(
    FrontendCApiRunnerCommandOptionParseResult result);

bool FrontendCApiRunnerOptionWasHandled(
    FrontendCApiRunnerCommandOptionParseResult result);

void InitializeFrontendCApiRunnerParsedOptions(
    char **argv,
    FrontendCApiRunnerOptions &options);

void NormalizeFrontendCApiRunnerParsedOptions(
    FrontendCApiRunnerOptions &options);

FrontendCApiRunnerCommandOptionParseResult
DispatchFrontendCApiRunnerCommandOption(const std::string &arg,
                                        int argc,
                                        char **argv,
                                        int &index,
                                        FrontendCApiRunnerOptions &options,
                                        std::string &error);

FrontendCApiRunnerCommandOptionParseResult
DispatchFrontendCApiRunnerPathRuntimeOptionPass(
    const std::string &arg,
    int argc,
    char **argv,
    int &index,
    FrontendCApiRunnerOptions &options,
    std::string &error);

FrontendCApiRunnerCommandOptionParseResult
DispatchFrontendCApiRunnerBackendOptionPass(const std::string &arg,
                                            int argc,
                                            char **argv,
                                            int &index,
                                            FrontendCApiRunnerOptions &options,
                                            std::string &error);

FrontendCApiRunnerCommandOptionParseResult
DispatchFrontendCApiRunnerEmissionHelpOptionPass(
    const std::string &arg,
    FrontendCApiRunnerOptions &options,
    std::string &error);
