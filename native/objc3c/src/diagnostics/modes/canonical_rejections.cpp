#include "diagnostics/modes/canonical_rejections.h"

#include "config/objc3_command_options.h"

namespace objc3c::diagnostics::modes {

CanonicalRejectionDiagnostic ClassifyCanonicalModeRejection(
    std::string_view flag) {
  const objc3c::config::ConfigValidationResult result =
      objc3c::config::ValidateRemovedCommandOption(flag);
  if (result.ok()) {
    return CanonicalRejectionDiagnostic{};
  }
  CanonicalRejectionDiagnostic diagnostic;
  diagnostic.matched = true;
  diagnostic.flag = std::string(flag);
  diagnostic.diagnostic_code = result.diagnostic_code;
  diagnostic.message = result.message;
  return diagnostic;
}

bool BuildCanonicalModeRejectionDiagnostic(const std::string &flag,
                                           std::string &diagnostic) {
  const CanonicalRejectionDiagnostic rejection =
      ClassifyCanonicalModeRejection(flag);
  if (!rejection.matched) {
    return false;
  }
  diagnostic = rejection.diagnostic_code + ": " + rejection.message;
  return true;
}

}  // namespace objc3c::diagnostics::modes
