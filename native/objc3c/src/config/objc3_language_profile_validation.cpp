#include "config/objc3_language_profile.h"

#include <string>

#include "config/objc3_config_parsing.h"

namespace objc3c::config {

ConfigValidationResult ValidateLanguageProfileName(std::string_view name) {
  if (IsCanonicalLanguageProfile(name)) {
    return MakeConfigValidationAccepted();
  }
  const std::string normalized = NormalizeLanguageProfileName(name);
  if (normalized == "strict") {
    return MakeConfigValidationRejected(
        "O3C036",
        "strict Objective-C language profile is reserved until match "
        "expression, strict diagnostics, textual interface, package replay, "
        "and public release evidence rows are complete; no compatibility mode "
        "or alias is available: " +
            std::string(name));
  }
  if (normalized == "strict-concurrency") {
    return MakeConfigValidationRejected(
        "O3C037",
        "strict-concurrency Objective-C language profile is reserved until "
        "actor isolation, sendability, task lifecycle, scheduler, and mailbox "
        "runtime evidence rows are complete; no compatibility mode or alias is "
        "available: " +
            std::string(name));
  }
  return MakeConfigValidationRejected(
      "O3C001",
      "unsupported Objective-C language profile for native frontend "
      "(expected canonical): " +
          std::string(name));
}

}  // namespace objc3c::config
