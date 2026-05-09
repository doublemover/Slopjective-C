#include "support/objc3_method_family_profiles.h"

#include "support/objc3_method_family.h"

namespace objc3c::support {

bool MethodFamilyReturnsRetainedResult(std::string_view family_name) {
  return family_name == "init" || family_name == "copy" ||
         family_name == "mutableCopy" || family_name == "new";
}

bool MethodFamilyReturnsRelatedResult(std::string_view family_name) {
  return family_name == "init";
}

bool IsKnownMethodFamilyName(std::string_view family_name) {
  return family_name == "init" || family_name == "copy" ||
         family_name == "mutableCopy" || family_name == "new" ||
         family_name == "none";
}

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
