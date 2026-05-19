#include "tools/objc3c_frontend_c_api_runner_command_parser_runtime_options_internal.h"

FrontendCApiRunnerCommandOptionParseResult
ParseFrontendCApiRunnerNumericRuntimeOptionPassSequence(
    const std::string &arg,
    int argc,
    char **argv,
    int &index,
    FrontendCApiRunnerOptions &options,
    std::string &error) {
  (void)error;
  if (arg == "--objc3-enable-live-error-runtime-surface") {
    options.allow_live_error_runtime_surface = true;
    return FrontendCApiRunnerCommandOptionParseResult::kHandled;
  }

  const FrontendCApiRunnerCommandOptionParseResult max_message_args_result =
      ParseFrontendCApiRunnerMaxMessageArgsOption(arg, argc, argv, index,
                                                  options, error);
  if (FrontendCApiRunnerRuntimeOptionShouldReturn(max_message_args_result)) {
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
  if (FrontendCApiRunnerRuntimeOptionShouldReturn(
          runtime_dispatch_symbol_result)) {
    return runtime_dispatch_symbol_result;
  }

  const FrontendCApiRunnerCommandOptionParseResult registration_ordinal_result =
      ParseFrontendCApiRunnerRegistrationOrdinalOption(arg, argc, argv, index,
                                                       options, error);
  if (FrontendCApiRunnerRuntimeOptionShouldReturn(registration_ordinal_result)) {
    return registration_ordinal_result;
  }

  return FrontendCApiRunnerCommandOptionParseResult::kNotHandled;
}
