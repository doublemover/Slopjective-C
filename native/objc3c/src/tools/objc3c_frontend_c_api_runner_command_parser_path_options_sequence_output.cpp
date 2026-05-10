#include "tools/objc3c_frontend_c_api_runner_command_parser_path_options_sequence_internal.h"

FrontendCApiRunnerCommandOptionParseResult
ParseFrontendCApiRunnerPathStringOutputPass(
    const std::string &arg,
    int argc,
    char **argv,
    int &index,
    FrontendCApiRunnerOptions &options) {
  return ParseFrontendCApiRunnerOutputPathStringOption(arg,
                                                       argc,
                                                       argv,
                                                       index,
                                                       options);
}
