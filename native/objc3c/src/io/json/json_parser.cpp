#include "io/json/json_parser.h"

#include <string>
#include <utility>

#include "io/json/json_parser_array_container.h"
#include "io/json/json_parser_cursor.h"
#include "io/json/json_parser_object_container.h"
#include "io/json/json_parser_value_delegate.h"

namespace objc3::io::json {
namespace {

class Parser : public JsonParserValueDelegate {
 public:
  explicit Parser(std::string_view text) : cursor_(text) {}

  JsonParseResult Parse() {
    JsonValue value;
    if (!ParseValue(value)) {
      return {JsonValue::Null(), cursor_.error()};
    }
    cursor_.SkipWhitespace();
    if (!cursor_.AtEnd()) {
      cursor_.Fail("unexpected trailing JSON content");
      return {JsonValue::Null(), cursor_.error()};
    }
    return {std::move(value), std::nullopt};
  }

 private:
  bool ParseValue(JsonValue &out) override {
    cursor_.SkipWhitespace();
    if (cursor_.AtEnd()) {
      return cursor_.Fail("unexpected end of JSON input");
    }
    const char ch = cursor_.Peek();
    if (ch == '{') {
      return ParseJsonObjectContainer(cursor_, *this, out);
    }
    if (ch == '[') {
      return ParseJsonArrayContainer(cursor_, *this, out);
    }
    if (ch == '"') {
      std::string value;
      if (!cursor_.ParseString(value)) {
        return false;
      }
      out = JsonValue::String(std::move(value));
      return true;
    }
    if (ch == 't') {
      return ParseLiteral("true", JsonValue::Bool(true), out);
    }
    if (ch == 'f') {
      return ParseLiteral("false", JsonValue::Bool(false), out);
    }
    if (ch == 'n') {
      return ParseLiteral("null", JsonValue::Null(), out);
    }
    if (ch == '-' || JsonParserCursor::IsDigit(ch)) {
      return cursor_.ParseNumber(out);
    }
    return cursor_.Fail("unexpected JSON token");
  }

  bool ParseLiteral(std::string_view literal, JsonValue value, JsonValue &out) {
    if (!cursor_.ConsumeLiteral(literal)) {
      return cursor_.Fail("invalid JSON literal");
    }
    out = std::move(value);
    return true;
  }

  JsonParserCursor cursor_;
};

}  // namespace

JsonParseResult ParseJson(std::string_view text) {
  return Parser(text).Parse();
}

}  // namespace objc3::io::json
