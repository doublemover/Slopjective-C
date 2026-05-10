#include "io/json/json_parser_object_container.h"

#include <string>
#include <utility>

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
    std::string key;
    if (!cursor.ParseString(key)) {
      return false;
    }
    cursor.SkipWhitespace();
    if (!cursor.Consume(':')) {
      return cursor.Fail("expected ':' after JSON object key");
    }
    JsonValue value;
    if (!value_parser.ParseValue(value)) {
      return false;
    }
    auto inserted = object.emplace(std::move(key), std::move(value));
    if (!inserted.second) {
      return cursor.Fail("duplicate JSON object key");
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
