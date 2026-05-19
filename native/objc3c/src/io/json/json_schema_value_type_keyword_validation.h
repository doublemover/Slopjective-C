#pragma once

#include "io/json/json_value.h"

namespace objc3::io::json {

[[nodiscard]] const JsonValue *FindJsonSchemaValueTypeKeyword(
    const JsonValue &schema);

}  // namespace objc3::io::json
