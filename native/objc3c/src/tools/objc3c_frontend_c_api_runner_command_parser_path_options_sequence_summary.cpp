#include "tools/objc3c_frontend_c_api_runner_command_parser_path_options_sequence_internal.h"

FrontendCApiRunnerCommandOptionParseResult
ParseFrontendCApiRunnerPathStringSummaryOutputPass(
    const std::string &arg,
    int argc,
    char **argv,
    int &index,
    FrontendCApiRunnerOptions &options) {
  return ParseFrontendCApiRunnerSummaryOutputPathStringOption(arg,
                                                             argc,
                                                             argv,
                                                             index,
                                                             options);
}
