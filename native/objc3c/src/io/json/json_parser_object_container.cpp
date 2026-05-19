#include "io/json/json_parser_object_container.h"

#include <utility>

#include "io/json/json_parser_object_container_member.h"

namespace objc3::io::json {

bool ParseJsonObjectContainer(JsonParserCursor &cursor,
                              JsonParserValueDelegate &value_parser,
                              JsonValue &out) {
  cursor.Consume('{');
  JsonValue::Object object;
  cursor.SkipWhitespace();
  if (cursor.Consume('}')) {
    out = JsonValue::ObjectValue(std::move(object));
    return true;
  }
  while (true) {
    if (!ParseJsonObjectContainerMember(cursor, value_parser, object)) {
      return false;
    }
    cursor.SkipWhitespace();
    if (cursor.Consume('}')) {
      out = JsonValue::ObjectValue(std::move(object));
      return true;
    }
    if (!cursor.Consume(',')) {
      return cursor.Fail("expected ',' or '}' in JSON object");
    }
    cursor.SkipWhitespace();
  }
}

}  // namespace objc3::io::json
