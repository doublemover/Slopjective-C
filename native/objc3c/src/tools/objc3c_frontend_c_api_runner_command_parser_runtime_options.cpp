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
  const FrontendCApiRunnerCommandOptionParseResult max_message_args_result =
      ParseFrontendCApiRunnerMaxMessageArgsOption(arg, argc, argv, index,
                                                  options, error);
  if (max_message_args_result !=
      FrontendCApiRunnerCommandOptionParseResult::kNotHandled) {
    return max_message_args_result;
  }

  const FrontendCApiRunnerCommandOptionParseResult
      runtime_dispatch_symbol_result =
          ParseFrontendCApiRunnerRuntimeDispatchSymbolOption(
              arg,
              argc,
              argv,
              index,
              options,
              error);
  if (runtime_dispatch_symbol_result !=
      FrontendCApiRunnerCommandOptionParseResult::kNotHandled) {
    return runtime_dispatch_symbol_result;
  }

  const FrontendCApiRunnerCommandOptionParseResult registration_ordinal_result =
      ParseFrontendCApiRunnerRegistrationOrdinalOption(arg, argc, argv, index,
                                                       options, error);
  if (registration_ordinal_result !=
      FrontendCApiRunnerCommandOptionParseResult::kNotHandled) {
    return registration_ordinal_result;
  }

  return FrontendCApiRunnerCommandOptionParseResult::kNotHandled;
}
