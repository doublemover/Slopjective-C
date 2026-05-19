#include "tools/objc3c_frontend_c_api_runner_command_parser_runtime_options_internal.h"

#include "tools/objc3c_frontend_c_api_runner_option_values.h"

FrontendCApiRunnerCommandOptionParseResult
ParseFrontendCApiRunnerRuntimeDispatchSymbolOption(
    const std::string &arg,
    int argc,
    char **argv,
    int &index,
    FrontendCApiRunnerOptions &options,
    std::string &error) {
  if (arg == "--objc3-runtime-dispatch-symbol" && index + 1 < argc) {
    if (!ParseFrontendCApiRunnerRuntimeDispatchSymbol(
            argv[++index],
            options.runtime_dispatch_symbol,
            error)) {
      return FrontendCApiRunnerCommandOptionParseResult::kError;
    }
    return FrontendCApiRunnerCommandOptionParseResult::kHandled;
  }
  return FrontendCApiRunnerCommandOptionParseResult::kNotHandled;
}
