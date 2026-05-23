#include "config/objc3_language_profile.h"

#include <string>

#include "config/objc3_config_parsing.h"

namespace objc3c::config {

namespace {

constexpr const char *kObjc3StrictProfileReservedDiagnosticCode = "O3C036";
constexpr const char *kObjc3StrictConcurrencyProfileReservedDiagnosticCode =
    "O3C037";
constexpr const char *kObjc3StrictSystemTargetOnlyProfileDiagnosticCode =
    "O3C038";
constexpr const char *kObjc3UnsupportedLanguageProfileDiagnosticCode =
    "O3C001";

}  // namespace

ConfigValidationResult ValidateLanguageProfileName(std::string_view name) {
  if (IsCanonicalLanguageProfile(name)) {
    return MakeConfigValidationAccepted();
  }
  const std::string normalized = NormalizeLanguageProfileName(name);
  if (normalized == "strict") {
    return MakeConfigValidationRejected(
        kObjc3StrictProfileReservedDiagnosticCode,
        "strict Objective-C language profile is reserved until match "
        "expression, strict diagnostics, textual interface, package replay, "
        "and public release evidence rows are complete; no compatibility mode "
        "or alias is available: " +
            std::string(name));
  }
  if (normalized == "strict-concurrency") {
    return MakeConfigValidationRejected(
        kObjc3StrictConcurrencyProfileReservedDiagnosticCode,
        "strict-concurrency Objective-C language profile is reserved until "
        "actor isolation, sendability, task lifecycle, scheduler, and mailbox "
        "runtime evidence rows are complete; no compatibility mode or alias is "
        "available: " +
            std::string(name));
  }
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
      "(expected canonical): " +
          std::string(name));
}

}  // namespace objc3c::config
