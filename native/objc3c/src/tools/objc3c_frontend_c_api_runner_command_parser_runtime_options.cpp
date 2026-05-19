#include "tools/objc3c_frontend_c_api_runner_command_parser_options.h"

#include "tools/objc3c_frontend_c_api_runner_command_parser_runtime_options_internal.h"

FrontendCApiRunnerCommandOptionParseResult
ParseFrontendCApiRunnerNumericRuntimeOption(
    const std::string &arg,
    int argc,
    char **argv,
    int &index,
    FrontendCApiRunnerOptions &options,
    std::string &error) {
  return ParseFrontendCApiRunnerNumericRuntimeOptionPassSequence(arg,
                                                                 argc,
                                                                 argv,
                                                                 index,
                                                                 options,
                                                                 error);
}
