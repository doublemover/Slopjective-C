#pragma once

#include "io/json/json_parser_cursor.h"
#include "io/json/json_parser_value_delegate.h"
#include "io/json/json_value.h"

namespace objc3::io::json {

bool ParseJsonObjectContainer(JsonParserCursor &cursor,
                              JsonParserValueDelegate &value_parser,
                              JsonValue &out);

}  // namespace objc3::io::json
