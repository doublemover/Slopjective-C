#include "config/objc3_language_profile.h"

#include "config/objc3_language_profile_table.h"

namespace objc3c::config {

const LanguageProfileContract &CanonicalLanguageProfile() {
  return LanguageProfileTable()[0];
}

}  // namespace objc3c::config
