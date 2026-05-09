#pragma once

#include <string>
#include <string_view>

#include "io/json/json_value.h"
#include "io/json/json_pointer_tokens.h"

namespace objc3::io::json {

const JsonValue *ResolveLocalJsonPointerRef(const JsonValue &schema_root,
                                            std::string_view ref);

}  // namespace objc3::io::json
