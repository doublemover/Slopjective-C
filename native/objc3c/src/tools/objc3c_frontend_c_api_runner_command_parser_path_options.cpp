#include "tools/objc3c_frontend_c_api_runner_command_parser_options.h"

#include "tools/objc3c_frontend_c_api_runner_command_parser_path_options_internal.h"

FrontendCApiRunnerCommandOptionParseResult
ParseFrontendCApiRunnerPathStringOption(const std::string &arg,
                                        int argc,
                                        char **argv,
                                        int &index,
                                        FrontendCApiRunnerOptions &options) {
  return ParseFrontendCApiRunnerPathStringOptionPassSequence(arg,
                                                             argc,
                                                             argv,
                                                             index,
                                                             options);
}
