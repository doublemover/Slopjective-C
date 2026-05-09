#include "config/objc3_language_profile.h"

namespace objc3c::config {
namespace {

constexpr LanguageProfileContract kCanonicalProfile{};

}  // namespace

const LanguageProfileContract &CanonicalLanguageProfile() {
  return kCanonicalProfile;
}

}  // namespace objc3c::config
