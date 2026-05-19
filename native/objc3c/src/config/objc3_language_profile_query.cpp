#include "config/objc3_language_profile.h"

#include <string>

#include "config/objc3_config_parsing.h"
#include "config/objc3_language_profile_table.h"

namespace objc3c::config {

const LanguageProfileContract *FindLanguageProfile(std::string_view name) {
  const std::string normalized = NormalizeLanguageProfileName(name);
  for (const LanguageProfileContract &profile : LanguageProfileTable()) {
    if (normalized == profile.name) {
      return &profile;
    }
  }
  return nullptr;
}

bool IsCanonicalLanguageProfile(std::string_view name) {
  return FindLanguageProfile(name) != nullptr;
}

}  // namespace objc3c::config
