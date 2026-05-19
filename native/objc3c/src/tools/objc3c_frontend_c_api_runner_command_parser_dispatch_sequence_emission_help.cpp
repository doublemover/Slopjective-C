#include "tools/objc3c_frontend_c_api_runner_command_parser_dispatch_sequence_internal.h"

#include "tools/objc3c_frontend_c_api_runner_command_parser_internal.h"

FrontendCApiRunnerCommandOptionParseResult
DispatchFrontendCApiRunnerCommandOptionEmissionHelpPass(
    const std::string &arg,
    FrontendCApiRunnerOptions &options,
    std::string &error) {
  return DispatchFrontendCApiRunnerEmissionHelpOptionPass(arg, options, error);
}
