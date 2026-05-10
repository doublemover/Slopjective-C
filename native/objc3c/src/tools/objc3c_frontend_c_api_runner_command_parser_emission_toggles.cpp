#include "tools/objc3c_frontend_c_api_runner_command_parser_dump_help_internal.h"

FrontendCApiRunnerCommandOptionParseResult
ParseFrontendCApiRunnerEmissionToggleOption(
    const std::string &arg,
    FrontendCApiRunnerOptions &options) {
  if (arg == "--no-emit-manifest") {
    options.emit_manifest = false;
    return FrontendCApiRunnerCommandOptionParseResult::kHandled;
  }
  if (arg == "--no-emit-ir") {
    options.emit_ir = false;
    return FrontendCApiRunnerCommandOptionParseResult::kHandled;
  }
  if (arg == "--no-emit-object") {
    options.emit_object = false;
    return FrontendCApiRunnerCommandOptionParseResult::kHandled;
  }
  return FrontendCApiRunnerCommandOptionParseResult::kNotHandled;
}
