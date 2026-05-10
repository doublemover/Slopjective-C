#include "io/json/json_parser_scalar_delegates.h"

#include <string>
#include <utility>

namespace objc3::io::json {

bool ParseJsonStringScalarValue(JsonParserCursor &cursor, JsonValue &out) {
  std::string value;
  if (!cursor.ParseString(value)) {
    return false;
  }
  out = JsonValue::String(std::move(value));
  return true;
}

bool ParseJsonNumberScalarValue(JsonParserCursor &cursor, JsonValue &out) {
  return cursor.ParseNumber(out);
}

}  // namespace objc3::io::json
