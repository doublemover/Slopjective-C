#include "tools/objc3c_frontend_c_api_runner_command_parser_internal.h"

bool FrontendCApiRunnerOptionParseFailed(
    FrontendCApiRunnerCommandOptionParseResult result) {
  return result == FrontendCApiRunnerCommandOptionParseResult::kError;
}

bool FrontendCApiRunnerOptionWasHandled(
    FrontendCApiRunnerCommandOptionParseResult result) {
  return result == FrontendCApiRunnerCommandOptionParseResult::kHandled;
}
