#include "tools/objc3c_frontend_c_api_runner_command_parser_dispatch_internal.h"

#include "tools/objc3c_frontend_c_api_runner_command_parser_internal.h"

bool FrontendCApiRunnerDispatchShouldReturn(
    FrontendCApiRunnerCommandOptionParseResult result) {
  return FrontendCApiRunnerOptionParseFailed(result) ||
         FrontendCApiRunnerOptionWasHandled(result);
}
