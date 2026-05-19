#include "config/objc3_language_version.h"

#include <string>

namespace objc3c::config {

ConfigValidationResult ValidateCanonicalLanguageVersion(
    std::uint32_t version) {
  if (IsCanonicalLanguageVersion(version)) {
    return MakeConfigValidationAccepted();
  }
  return MakeConfigValidationRejected(
      "O3C001", UnsupportedLanguageVersionDiagnostic(version));
}

ConfigValidationResult ValidateCanonicalLanguageVersionText(
    std::string_view text) {
  std::uint32_t version = 0;
  if (!ParseLanguageVersionText(text, version)) {
    return MakeConfigValidationInvalid(
        "O3C001",
        "invalid Objective-C language version for native frontend: " +
            std::string(text));
  }
  return ValidateCanonicalLanguageVersion(version);
}

}  // namespace objc3c::config
