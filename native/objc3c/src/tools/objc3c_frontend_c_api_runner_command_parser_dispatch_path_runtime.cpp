#include "tools/objc3c_frontend_c_api_runner_command_parser_internal.h"

FrontendCApiRunnerCommandOptionParseResult
DispatchFrontendCApiRunnerPathRuntimeOptionPass(
    const std::string &arg,
    int argc,
    char **argv,
    int &index,
    FrontendCApiRunnerOptions &options,
    std::string &error) {
  const FrontendCApiRunnerCommandOptionParseResult path_string_result =
      ParseFrontendCApiRunnerPathStringOption(arg, argc, argv, index, options);
  if (FrontendCApiRunnerOptionWasHandled(path_string_result)) {
    return path_string_result;
  }

  return ParseFrontendCApiRunnerNumericRuntimeOption(arg, argc, argv, index,
                                                     options, error);
}
