#include "tools/objc3c_frontend_c_api_runner_command_parser_options.h"

#include "tools/objc3c_frontend_c_api_runner_option_values.h"

FrontendCApiRunnerCommandOptionParseResult
ParseFrontendCApiRunnerNumericRuntimeOption(
    const std::string &arg,
    int argc,
    char **argv,
    int &index,
    FrontendCApiRunnerOptions &options,
    std::string &error) {
  if (arg == "--objc3-max-message-args" && index + 1 < argc) {
    if (!ParseFrontendCApiRunnerMaxMessageSendArgs(
            argv[++index],
            options.max_message_send_args,
            error)) {
      return FrontendCApiRunnerCommandOptionParseResult::kError;
    }
    return FrontendCApiRunnerCommandOptionParseResult::kHandled;
  }
  if (arg == "--objc3-runtime-dispatch-symbol" && index + 1 < argc) {
    if (!ParseFrontendCApiRunnerRuntimeDispatchSymbol(
            argv[++index],
            options.runtime_dispatch_symbol,
            error)) {
      return FrontendCApiRunnerCommandOptionParseResult::kError;
    }
    return FrontendCApiRunnerCommandOptionParseResult::kHandled;
  }
  if (arg == "--objc3-bootstrap-registration-order-ordinal" &&
      index + 1 < argc) {
    if (!ParseFrontendCApiRunnerRegistrationOrderOrdinal(
            argv[++index],
            options.translation_unit_registration_order_ordinal,
            error)) {
      return FrontendCApiRunnerCommandOptionParseResult::kError;
    }
    return FrontendCApiRunnerCommandOptionParseResult::kHandled;
  }
  return FrontendCApiRunnerCommandOptionParseResult::kNotHandled;
}
