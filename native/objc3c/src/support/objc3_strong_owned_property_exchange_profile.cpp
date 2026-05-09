#include "support/objc3_strong_owned_property_exchange_profile.h"

#include "support/objc3_property_attribute_profile_match.h"
#include "support/objc3_property_ownership_profile_tokens.h"

namespace objc3c::support {

bool UsesStrongOwnedCurrentPropertyExchange(
    std::string_view ownership_lifetime_profile,
    std::string_view accessor_ownership_profile) {
  return ownership_lifetime_profile ==
             kObjc3PropertyStrongOwnedLifetimeProfile ||
         PropertyAttributeProfileContains(accessor_ownership_profile,
                                          "ownership_lifetime=strong-owned");
}

}  // namespace objc3c::support
