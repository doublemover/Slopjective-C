#include "tools/objc3c_frontend_c_api_runner_command_parser_runtime_options_internal.h"

bool FrontendCApiRunnerRuntimeOptionShouldReturn(
    FrontendCApiRunnerCommandOptionParseResult result) {
  return result != FrontendCApiRunnerCommandOptionParseResult::kNotHandled;
}
