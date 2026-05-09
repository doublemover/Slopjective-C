#include "support/objc3_profile_any_token_match.h"

#include "support/objc3_profile_single_token_match.h"

namespace objc3c::support {

bool ProfileContainsAnyToken(
    std::string_view profile,
    std::initializer_list<std::string_view> tokens) {
  for (const std::string_view token : tokens) {
    if (ProfileContainsToken(profile, token)) {
      return true;
    }
  }
  return false;
}

}  // namespace objc3c::support
