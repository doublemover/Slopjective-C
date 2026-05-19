#pragma once

#include "io/json/json_parser_cursor.h"
#include "io/json/json_value.h"

namespace objc3::io::json {

bool ParseJsonScalarValue(JsonParserCursor &cursor, JsonValue &out);

}  // namespace objc3::io::json
