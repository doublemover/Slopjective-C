#include "config/objc3_language_profile.h"

#include "config/objc3_config_parsing.h"

namespace objc3c::config {

const LanguageProfileContract *FindLanguageProfile(std::string_view name) {
  if (NormalizeLanguageProfileName(name) == kCanonicalLanguageProfileName) {
    return &CanonicalLanguageProfile();
  }
  return nullptr;
}

bool IsCanonicalLanguageProfile(std::string_view name) {
  return FindLanguageProfile(name) != nullptr;
}

}  // namespace objc3c::config
