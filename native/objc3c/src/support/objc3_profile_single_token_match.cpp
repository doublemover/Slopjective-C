#include "support/objc3_profile_single_token_match.h"

#include "support/objc3_string_search_predicates.h"

namespace objc3c::support {

bool ProfileContainsToken(std::string_view profile,
                          std::string_view token) {
  return Contains(profile, token);
}

}  // namespace objc3c::support
