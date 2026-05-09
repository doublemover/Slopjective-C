#include "support/objc3_property_ownership_classification.h"

#include "support/objc3_profile_token_match.h"
#include "support/objc3_property_profile_tokens.h"

namespace objc3c::support {

bool HasArcRuntimeOwnedPropertyAttribute(
    bool has_strong_attribute,
    bool has_retain_attribute,
    bool has_copy_attribute,
    bool has_weak_attribute,
    std::string_view property_attribute_profile) {
  return has_strong_attribute || has_retain_attribute || has_copy_attribute ||
         has_weak_attribute ||
         ProfileContainsAnyToken(
             property_attribute_profile,
             {"strong=1", "retain=1", "copy=1", "weak=1"});
}

Objc3PropertyOwnershipProfileKind ClassifyPropertyOwnershipProfile(
    bool has_weak_ownership,
    bool has_strong_ownership,
    bool has_retain_ownership,
    bool has_copy_ownership,
    std::string_view property_attribute_profile,
    bool include_retain_profile) {
  if (has_weak_ownership ||
      PropertyAttributeProfileContains(property_attribute_profile, "weak=1")) {
    return Objc3PropertyOwnershipProfileKind::kWeak;
  }
  if (has_strong_ownership || has_retain_ownership || has_copy_ownership ||
      ProfileContainsAnyToken(property_attribute_profile,
                              {"strong=1", "copy=1"}) ||
      (include_retain_profile &&
       PropertyAttributeProfileContains(property_attribute_profile,
                                        "retain=1"))) {
    return Objc3PropertyOwnershipProfileKind::kStrongOwned;
  }
  return Objc3PropertyOwnershipProfileKind::kNone;
}

}  // namespace objc3c::support
