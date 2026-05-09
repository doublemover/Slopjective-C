#pragma once

#include <string>
#include <string_view>

namespace objc3c::support {

struct Objc3MethodFamilyProfile {
  std::string name = "none";
  bool returns_retained_result = false;
  bool returns_related_result = false;
  bool normalized = false;
};

bool MethodFamilyReturnsRetainedResult(std::string_view family_name);
bool MethodFamilyReturnsRelatedResult(std::string_view family_name);
bool IsKnownMethodFamilyName(std::string_view family_name);
Objc3MethodFamilyProfile BuildMethodFamilyProfile(
    std::string_view selector,
    bool existing_normalized,
    const std::string &existing_family_name,
    bool existing_returns_retained_result,
    bool existing_returns_related_result);

}  // namespace objc3c::support
