#pragma once

#include <string>
#include <vector>

namespace objc3c::parse {

std::string BuildCleanupAttributeProfile(
    bool declared,
    const std::string &cleanup_symbol);

std::string BuildResourceAttributeProfile(
    bool declared,
    const std::string &close_symbol,
    const std::string &invalid_expression);

std::string BuildRetainableCFamilyCallableProfile(
    const std::vector<std::string> &attribute_names,
    const std::vector<std::string> &family_names);

}  // namespace objc3c::parse
