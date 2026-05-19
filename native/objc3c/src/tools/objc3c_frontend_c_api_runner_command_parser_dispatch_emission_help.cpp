#include "tools/objc3c_frontend_c_api_runner_command_parser_internal.h"

FrontendCApiRunnerCommandOptionParseResult
DispatchFrontendCApiRunnerEmissionHelpOptionPass(
    const std::string &arg,
    FrontendCApiRunnerOptions &options,
    std::string &error) {
  return ParseFrontendCApiRunnerEmissionDumpHelpOption(arg, options, error);
}
