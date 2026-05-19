#pragma once

#include <ostream>
#include <string>
#include <string_view>

namespace objc3::io::json {

std::string EscapeJsonStringContent(std::string_view value);
void WriteJsonStringContent(std::ostream &out, std::string_view value);

}  // namespace objc3::io::json
