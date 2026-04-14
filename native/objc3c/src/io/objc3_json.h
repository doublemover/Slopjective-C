#pragma once

#include <ostream>
#include <string>
#include <string_view>

namespace objc3::io {

std::string EscapeJsonString(std::string_view value);
void WriteJsonString(std::ostream &out, std::string_view value);

}  // namespace objc3::io
