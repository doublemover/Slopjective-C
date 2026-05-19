#pragma once

#include <string_view>

namespace objc3::io::json {

bool IsJsonSchemaAssertionKeyword(std::string_view key);

}  // namespace objc3::io::json
