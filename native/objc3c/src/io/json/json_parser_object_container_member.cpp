#include "io/json/json_parser_object_container_member.h"

#include <string>
#include <utility>

namespace objc3::io::json {

bool ParseJsonObjectContainerMember(JsonParserCursor &cursor,
                                    JsonParserValueDelegate &value_parser,
                                    JsonValue::Object &object) {
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
  return true;
}

}  // namespace objc3::io::json
