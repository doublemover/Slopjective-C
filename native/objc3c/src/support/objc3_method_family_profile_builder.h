#pragma once

#include <string>
#include <string_view>

#include "support/objc3_method_family_profile_record.h"

namespace objc3c::support {

Objc3MethodFamilyProfile BuildMethodFamilyProfile(
    std::string_view selector,
    bool existing_normalized,
    const std::string &existing_family_name,
    bool existing_returns_retained_result,
    bool existing_returns_related_result);

}  // namespace objc3c::support
