#include "config/objc3_language_profile.h"

#include <string>

namespace objc3c::config {

ConfigValidationResult ValidateLanguageProfileName(std::string_view name) {
  if (IsCanonicalLanguageProfile(name)) {
    return MakeConfigValidationAccepted();
  }
  return MakeConfigValidationRejected(
      "O3C001",
      "unsupported Objective-C language profile for native frontend "
      "(expected canonical): " +
          std::string(name));
}

}  // namespace objc3c::config
