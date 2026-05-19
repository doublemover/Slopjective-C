#pragma once

#include "io/json/json_parser_cursor.h"
#include "io/json/json_value.h"

namespace objc3::io::json {

bool ParseJsonStringScalarValue(JsonParserCursor &cursor, JsonValue &out);
bool ParseJsonNumberScalarValue(JsonParserCursor &cursor, JsonValue &out);

}  // namespace objc3::io::json
