#include "config/objc3_language_version.h"

#include "config/objc3_config_parsing.h"

namespace objc3c::config {

std::string UnsupportedLanguageVersionDiagnostic(std::uint32_t version) {
  return "unsupported Objective-C language version for native frontend "
         "(expected " +
         std::to_string(static_cast<unsigned>(kCanonicalLanguageVersion)) +
         "): " + std::to_string(version);
}

ConfigValidationResult ValidateCanonicalLanguageVersion(
    std::uint32_t version) {
  if (IsCanonicalLanguageVersion(version)) {
    return MakeConfigValidationAccepted();
  }
  return MakeConfigValidationRejected(
      "O3C001", UnsupportedLanguageVersionDiagnostic(version));
}

bool ParseLanguageVersionText(std::string_view text, std::uint32_t &version) {
  return ParseUnsignedConfigValue(text, version);
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
