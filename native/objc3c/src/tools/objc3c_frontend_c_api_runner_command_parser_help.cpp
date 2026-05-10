#include "tools/objc3c_frontend_c_api_runner_command_parser_dump_help_internal.h"

#include "tools/objc3c_frontend_c_api_runner_usage.h"

FrontendCApiRunnerCommandOptionParseResult
ParseFrontendCApiRunnerHelpOption(const std::string &arg,
                                  std::string &error) {
  if (arg == "--help" || arg == "-h") {
    error = FrontendCApiRunnerUsage();
    return FrontendCApiRunnerCommandOptionParseResult::kError;
  }
  return FrontendCApiRunnerCommandOptionParseResult::kNotHandled;
}
