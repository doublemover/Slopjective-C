#include "config/objc3_language_profile.h"

#include "config/objc3_config_parsing.h"

namespace objc3c::config {

namespace {

constexpr LanguageProfileContract kCanonicalProfile{};

}  // namespace

const LanguageProfileContract &CanonicalLanguageProfile() {
  return kCanonicalProfile;
}

const LanguageProfileContract *FindLanguageProfile(std::string_view name) {
  if (NormalizeLanguageProfileName(name) == kCanonicalLanguageProfileName) {
    return &kCanonicalProfile;
  }
  return nullptr;
}

bool IsCanonicalLanguageProfile(std::string_view name) {
  return FindLanguageProfile(name) != nullptr;
}

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
