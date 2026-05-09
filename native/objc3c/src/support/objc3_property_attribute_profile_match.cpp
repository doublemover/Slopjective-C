#include "support/objc3_property_attribute_profile_match.h"

#include "support/objc3_profile_token_match.h"

namespace objc3c::support {

bool PropertyAttributeProfileContains(std::string_view profile,
                                      std::string_view needle) {
  return ProfileContainsToken(profile, needle);
}

}  // namespace objc3c::support
