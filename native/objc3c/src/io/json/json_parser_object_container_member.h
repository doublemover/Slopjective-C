#pragma once

#include "io/json/json_parser_cursor.h"
#include "io/json/json_parser_value_delegate.h"
#include "io/json/json_value.h"

namespace objc3::io::json {

bool ParseJsonObjectContainerMember(JsonParserCursor &cursor,
                                    JsonParserValueDelegate &value_parser,
                                    JsonValue::Object &object);

}  // namespace objc3::io::json
