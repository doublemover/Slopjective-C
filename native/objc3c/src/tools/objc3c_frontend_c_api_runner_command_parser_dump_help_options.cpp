#include "tools/objc3c_frontend_c_api_runner_command_parser_options.h"

#include "tools/objc3c_frontend_c_api_runner_command_parser_dump_help_internal.h"

FrontendCApiRunnerCommandOptionParseResult
ParseFrontendCApiRunnerEmissionDumpHelpOption(
    const std::string &arg,
    FrontendCApiRunnerOptions &options,
    std::string &error) {
  const FrontendCApiRunnerCommandOptionParseResult emission_toggle_result =
      ParseFrontendCApiRunnerEmissionToggleOption(arg, options);
  if (emission_toggle_result !=
      FrontendCApiRunnerCommandOptionParseResult::kNotHandled) {
    return emission_toggle_result;
  }

  const FrontendCApiRunnerCommandOptionParseResult dump_flag_result =
      ParseFrontendCApiRunnerDumpFlagOption(arg, options);
  if (dump_flag_result !=
      FrontendCApiRunnerCommandOptionParseResult::kNotHandled) {
    return dump_flag_result;
  }

  return ParseFrontendCApiRunnerHelpOption(arg, error);
}
