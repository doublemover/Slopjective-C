#include "tools/objc3c_frontend_c_api_runner_command_parser_dump_help_internal.h"

#include "tools/objc3c_frontend_c_api_runner_dump_options.h"

FrontendCApiRunnerCommandOptionParseResult
ParseFrontendCApiRunnerDumpFlagOption(const std::string &arg,
                                      FrontendCApiRunnerOptions &options) {
  if (ApplyFrontendCApiRunnerDumpOption(arg, options)) {
    return FrontendCApiRunnerCommandOptionParseResult::kHandled;
  }
  return FrontendCApiRunnerCommandOptionParseResult::kNotHandled;
}
