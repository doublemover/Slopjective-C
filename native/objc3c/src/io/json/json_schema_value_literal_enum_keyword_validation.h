#pragma once

#include "io/json/json_value.h"

namespace objc3::io::json {

[[nodiscard]] const JsonValue *ValidateJsonSchemaEnumLiteralKeywordArray(
    const JsonValue &schema);

}  // namespace objc3::io::json
