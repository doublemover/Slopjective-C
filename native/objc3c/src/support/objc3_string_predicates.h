#pragma once

#include <string>
#include <string_view>

namespace objc3c::support {

std::string LowercaseAscii(std::string_view value);
bool StartsWith(std::string_view value, std::string_view prefix);
bool EndsWith(std::string_view value, std::string_view suffix);

}  // namespace objc3c::support
