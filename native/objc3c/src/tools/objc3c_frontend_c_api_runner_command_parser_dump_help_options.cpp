#include "tools/objc3c_frontend_c_api_runner_command_parser_options.h"

#include "diagnostics/modes/objc3_removed_mode_options.h"
#include "tools/objc3c_frontend_c_api_runner_dump_options.h"
#include "tools/objc3c_frontend_c_api_runner_usage.h"

FrontendCApiRunnerCommandOptionParseResult
ParseFrontendCApiRunnerRemovedModeOption(const std::string &arg,
                                         std::string &error) {
  if (objc3c::diagnostics::modes::BuildRemovedModeOptionDiagnostic(arg,
                                                                   error)) {
    return FrontendCApiRunnerCommandOptionParseResult::kError;
  }
  return FrontendCApiRunnerCommandOptionParseResult::kNotHandled;
}

FrontendCApiRunnerCommandOptionParseResult
ParseFrontendCApiRunnerEmissionDumpHelpOption(
    const std::string &arg,
    FrontendCApiRunnerOptions &options,
    std::string &error) {
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
  if (ApplyFrontendCApiRunnerDumpOption(arg, options)) {
    return FrontendCApiRunnerCommandOptionParseResult::kHandled;
  }
  if (arg == "--help" || arg == "-h") {
    error = FrontendCApiRunnerUsage();
    return FrontendCApiRunnerCommandOptionParseResult::kError;
  }
  return FrontendCApiRunnerCommandOptionParseResult::kNotHandled;
}
