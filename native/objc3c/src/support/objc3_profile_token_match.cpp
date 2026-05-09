#include "support/objc3_profile_token_match.h"

#include "support/objc3_string_predicates.h"

namespace objc3c::support {

bool ProfileContainsToken(std::string_view profile,
                          std::string_view token) {
  return Contains(profile, token);
}

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

bool LowercaseProfileContainsAnyToken(
    std::string_view profile,
    std::initializer_list<std::string_view> tokens) {
  return ProfileContainsAnyToken(LowercaseAscii(profile), tokens);
}

}  // namespace objc3c::support
