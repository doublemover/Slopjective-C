#include "config/objc3_language_profile.h"

#include <string>

#include "config/objc3_config_parsing.h"

namespace objc3c::config {

namespace {

constexpr const char *kObjc3StrictSystemTargetOnlyProfileDiagnosticCode =
    "O3C038";
constexpr const char *kObjc3UnsupportedLanguageProfileDiagnosticCode =
    "O3C001";

}  // namespace

ConfigValidationResult ValidateLanguageProfileName(std::string_view name) {
  if (IsSupportedLanguageProfile(name)) {
    return MakeConfigValidationAccepted();
  }
  const std::string normalized = NormalizeLanguageProfileName(name);
  if (normalized == "strict-system") {
    return MakeConfigValidationRejected(
        kObjc3StrictSystemTargetOnlyProfileDiagnosticCode,
        "strict-system is a targeted release-evidence profile, not a native "
        "frontend language profile; no compatibility mode or alias is "
        "available: " +
            std::string(name));
  }
  return MakeConfigValidationRejected(
      kObjc3UnsupportedLanguageProfileDiagnosticCode,
      "unsupported Objective-C language profile for native frontend "
      "(expected canonical|strict|strict-concurrency): " +
          std::string(name));
}

}  // namespace objc3c::config
