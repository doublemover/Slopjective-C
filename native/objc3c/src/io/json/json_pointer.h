#pragma once

#include <string>
#include <string_view>

#include "io/json/json_value.h"

namespace objc3::io::json {

std::string DecodeJsonPointerToken(std::string_view token);
const JsonValue *ResolveLocalJsonPointerRef(const JsonValue &schema_root,
                                            std::string_view ref);

}  // namespace objc3::io::json
