#include "io/json/json_parser_scalar_value.h"

#include <string>
#include <string_view>
#include <utility>

namespace objc3::io::json {
namespace {

bool ParseLiteral(JsonParserCursor &cursor,
                  std::string_view literal,
                  JsonValue value,
                  JsonValue &out) {
  if (!cursor.ConsumeLiteral(literal)) {
    return cursor.Fail("invalid JSON literal");
  }
  out = std::move(value);
  return true;
}

}  // namespace

bool ParseJsonScalarValue(JsonParserCursor &cursor, JsonValue &out) {
  const char ch = cursor.Peek();
  if (ch == '"') {
    std::string value;
    if (!cursor.ParseString(value)) {
      return false;
    }
    out = JsonValue::String(std::move(value));
    return true;
  }
  if (ch == 't') {
    return ParseLiteral(cursor, "true", JsonValue::Bool(true), out);
  }
  if (ch == 'f') {
    return ParseLiteral(cursor, "false", JsonValue::Bool(false), out);
  }
  if (ch == 'n') {
    return ParseLiteral(cursor, "null", JsonValue::Null(), out);
  }
  if (ch == '-' || JsonParserCursor::IsDigit(ch)) {
    return cursor.ParseNumber(out);
  }
  return cursor.Fail("unexpected JSON token");
}

}  // namespace objc3::io::json
