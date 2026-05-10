#include "io/json/json_parser_array_container.h"

#include <utility>

namespace objc3::io::json {

bool ParseJsonArrayContainer(JsonParserCursor &cursor,
                             JsonParserValueDelegate &value_parser,
                             JsonValue &out) {
  cursor.Consume('[');
  JsonValue::Array array;
  cursor.SkipWhitespace();
  if (cursor.Consume(']')) {
    out = JsonValue::ArrayValue(std::move(array));
    return true;
  }
  while (true) {
    JsonValue value;
    if (!value_parser.ParseValue(value)) {
      return false;
    }
    array.push_back(std::move(value));
    cursor.SkipWhitespace();
    if (cursor.Consume(']')) {
      out = JsonValue::ArrayValue(std::move(array));
      return true;
    }
    if (!cursor.Consume(',')) {
      return cursor.Fail("expected ',' or ']' in JSON array");
    }
    cursor.SkipWhitespace();
  }
}

}  // namespace objc3::io::json
