#pragma once

#include "io/json/json_parser.h"
#include "io/json/json_parser_cursor.h"

namespace objc3::io::json {

JsonParseResult CompleteJsonParse(JsonParserCursor &cursor, JsonValue value);

}  // namespace objc3::io::json
