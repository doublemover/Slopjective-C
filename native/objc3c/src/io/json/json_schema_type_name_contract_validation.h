#pragma once

#include <string_view>

namespace objc3::io::json {

bool IsSupportedJsonSchemaTypeName(std::string_view type);

}  // namespace objc3::io::json
