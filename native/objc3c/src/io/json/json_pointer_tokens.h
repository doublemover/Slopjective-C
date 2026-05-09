#pragma once

#include <string>
#include <string_view>

namespace objc3::io::json {

std::string DecodeJsonPointerToken(std::string_view token);

}  // namespace objc3::io::json
