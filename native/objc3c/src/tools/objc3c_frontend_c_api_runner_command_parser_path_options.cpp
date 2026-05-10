#include "tools/objc3c_frontend_c_api_runner_command_parser_options.h"

#include "tools/objc3c_frontend_c_api_runner_command_parser_path_options_internal.h"

FrontendCApiRunnerCommandOptionParseResult
ParseFrontendCApiRunnerPathStringOption(const std::string &arg,
                                        int argc,
                                        char **argv,
                                        int &index,
                                        FrontendCApiRunnerOptions &options) {
  const FrontendCApiRunnerCommandOptionParseResult output_result =
      ParseFrontendCApiRunnerOutputPathStringOption(arg,
                                                    argc,
                                                    argv,
                                                    index,
                                                    options);
  if (output_result !=
      FrontendCApiRunnerCommandOptionParseResult::kNotHandled) {
    return output_result;
  }

  const FrontendCApiRunnerCommandOptionParseResult toolchain_result =
      ParseFrontendCApiRunnerToolchainPathStringOption(arg,
                                                       argc,
                                                       argv,
                                                       index,
                                                       options);
  if (toolchain_result !=
      FrontendCApiRunnerCommandOptionParseResult::kNotHandled) {
    return toolchain_result;
  }

  return ParseFrontendCApiRunnerSummaryOutputPathStringOption(arg,
                                                             argc,
                                                             argv,
                                                             index,
                                                             options);
}
