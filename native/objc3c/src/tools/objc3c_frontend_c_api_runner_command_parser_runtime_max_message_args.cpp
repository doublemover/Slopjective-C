#include "tools/objc3c_frontend_c_api_runner_command_parser_runtime_options_internal.h"

#include "tools/objc3c_frontend_c_api_runner_option_values.h"

FrontendCApiRunnerCommandOptionParseResult
ParseFrontendCApiRunnerMaxMessageArgsOption(
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
  return FrontendCApiRunnerCommandOptionParseResult::kNotHandled;
}
