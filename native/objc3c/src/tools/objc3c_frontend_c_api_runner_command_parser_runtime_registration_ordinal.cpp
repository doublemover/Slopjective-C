#include "tools/objc3c_frontend_c_api_runner_command_parser_runtime_options_internal.h"

#include "tools/objc3c_frontend_c_api_runner_option_values.h"

FrontendCApiRunnerCommandOptionParseResult
ParseFrontendCApiRunnerRegistrationOrdinalOption(
    const std::string &arg,
    int argc,
    char **argv,
    int &index,
    FrontendCApiRunnerOptions &options,
    std::string &error) {
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
