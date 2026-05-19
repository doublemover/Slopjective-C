#include "tools/objc3c_frontend_c_api_runner_command_parser_path_options_internal.h"

bool FrontendCApiRunnerPathOptionShouldReturn(
    FrontendCApiRunnerCommandOptionParseResult result) {
  return result != FrontendCApiRunnerCommandOptionParseResult::kNotHandled;
}
