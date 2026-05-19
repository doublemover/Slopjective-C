#pragma once

#include <string>

#include "tools/objc3c_frontend_c_api_runner_options.h"

enum class FrontendCApiRunnerCommandOptionParseResult {
  kNotHandled,
  kHandled,
  kError,
};

FrontendCApiRunnerCommandOptionParseResult
ParseFrontendCApiRunnerPathStringOption(const std::string &arg,
                                        int argc,
                                        char **argv,
                                        int &index,
                                        FrontendCApiRunnerOptions &options);

FrontendCApiRunnerCommandOptionParseResult
ParseFrontendCApiRunnerNumericRuntimeOption(
    const std::string &arg,
    int argc,
    char **argv,
    int &index,
    FrontendCApiRunnerOptions &options,
    std::string &error);

FrontendCApiRunnerCommandOptionParseResult
ParseFrontendCApiRunnerBackendSelectionOption(
    const std::string &arg,
    int argc,
    char **argv,
    int &index,
    FrontendCApiRunnerOptions &options,
    std::string &error);

FrontendCApiRunnerCommandOptionParseResult
ParseFrontendCApiRunnerRemovedModeOption(const std::string &arg,
                                         std::string &error);

FrontendCApiRunnerCommandOptionParseResult
ParseFrontendCApiRunnerEmissionDumpHelpOption(
    const std::string &arg,
    FrontendCApiRunnerOptions &options,
    std::string &error);
