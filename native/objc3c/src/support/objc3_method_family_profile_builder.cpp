#include "support/objc3_method_family_profile_builder.h"

#include "support/objc3_method_family.h"
#include "support/objc3_method_family_traits.h"

namespace objc3c::support {

Objc3MethodFamilyProfile BuildMethodFamilyProfile(
    std::string_view selector,
    bool existing_normalized,
    const std::string &existing_family_name,
    bool existing_returns_retained_result,
    bool existing_returns_related_result) {
  Objc3MethodFamilyProfile profile;
  profile.name =
      existing_normalized && !existing_family_name.empty()
          ? existing_family_name
          : ClassifyMethodFamilyFromSelector(std::string(selector));
  profile.returns_retained_result =
      existing_normalized ? existing_returns_retained_result
                          : MethodFamilyReturnsRetainedResult(profile.name);
  profile.returns_related_result =
      existing_normalized ? existing_returns_related_result
                          : MethodFamilyReturnsRelatedResult(profile.name);
  profile.normalized =
      existing_normalized ||
      (IsKnownMethodFamilyName(profile.name) &&
       (!profile.returns_related_result || profile.name == "init"));
  return profile;
}

}  // namespace objc3c::support
