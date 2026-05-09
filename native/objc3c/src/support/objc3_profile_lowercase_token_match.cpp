#include "support/objc3_profile_lowercase_token_match.h"

#include "support/objc3_ascii_case_transform.h"
#include "support/objc3_profile_any_token_match.h"

namespace objc3c::support {

bool LowercaseProfileContainsAnyToken(
    std::string_view profile,
    std::initializer_list<std::string_view> tokens) {
  return ProfileContainsAnyToken(LowercaseAscii(profile), tokens);
}

}  // namespace objc3c::support
