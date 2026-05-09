#pragma once

#include <string>
#include <string_view>

namespace objc3c::support {

std::string MakeIdentifierSafeSuffix(std::string_view text,
                                     std::string_view empty_replacement);

}  // namespace objc3c::support
