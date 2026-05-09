#include "diagnostics/modes/objc3_removed_mode_options.h"

#include "config/objc3_language_profile.h"

namespace objc3c::diagnostics::modes {

RemovedModeOptionDiagnostic ClassifyRemovedModeOption(std::string_view flag) {
  const objc3c::config::ConfigValidationResult result =
      objc3c::config::ValidateRemovedCommandOption(flag);
  if (result.ok()) {
    return RemovedModeOptionDiagnostic{};
  }
  RemovedModeOptionDiagnostic diagnostic;
  diagnostic.matched = true;
  diagnostic.flag = std::string(flag);
  diagnostic.diagnostic_code = result.diagnostic_code;
  diagnostic.message = result.message;
  return diagnostic;
}

bool BuildRemovedModeOptionDiagnostic(const std::string &flag,
                                      std::string &diagnostic) {
  const RemovedModeOptionDiagnostic removed =
      ClassifyRemovedModeOption(flag);
  if (!removed.matched) {
    return false;
  }
  diagnostic = removed.message;
  return true;
}

}  // namespace objc3c::diagnostics::modes
