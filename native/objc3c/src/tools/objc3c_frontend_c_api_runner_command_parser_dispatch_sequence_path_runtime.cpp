#include "tools/objc3c_frontend_c_api_runner_command_parser_dispatch_sequence_internal.h"

#include "tools/objc3c_frontend_c_api_runner_command_parser_internal.h"

FrontendCApiRunnerCommandOptionParseResult
DispatchFrontendCApiRunnerCommandOptionPathRuntimePass(
    const std::string &arg,
    int argc,
    char **argv,
    int &index,
    FrontendCApiRunnerOptions &options,
    std::string &error) {
  return DispatchFrontendCApiRunnerPathRuntimeOptionPass(arg,
                                                         argc,
                                                         argv,
                                                         index,
                                                         options,
                                                         error);
}
