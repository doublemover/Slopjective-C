#include "tools/objc3c_frontend_c_api_runner_command_parser_options.h"

#include "diagnostics/modes/canonical_rejections.h"

FrontendCApiRunnerCommandOptionParseResult
ParseFrontendCApiRunnerRemovedModeOption(const std::string &arg,
                                         std::string &error) {
  if (objc3c::diagnostics::modes::BuildCanonicalModeRejectionDiagnostic(
          arg, error)) {
    return FrontendCApiRunnerCommandOptionParseResult::kError;
  }
  return FrontendCApiRunnerCommandOptionParseResult::kNotHandled;
}
