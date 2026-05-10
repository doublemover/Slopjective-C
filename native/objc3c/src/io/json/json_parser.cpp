#include "io/json/json_parser.h"

#include <string>
#include <utility>

#include "io/json/json_parser_cursor.h"

namespace objc3::io::json {
namespace {

class Parser {
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
  bool ParseValue(JsonValue &out) {
    cursor_.SkipWhitespace();
    if (cursor_.AtEnd()) {
      return cursor_.Fail("unexpected end of JSON input");
    }
    const char ch = cursor_.Peek();
    if (ch == '{') {
      return ParseObject(out);
    }
    if (ch == '[') {
      return ParseArray(out);
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

  bool ParseObject(JsonValue &out) {
    cursor_.Consume('{');
    JsonValue::Object object;
    cursor_.SkipWhitespace();
    if (cursor_.Consume('}')) {
      out = JsonValue::ObjectValue(std::move(object));
      return true;
    }
    while (true) {
      std::string key;
      if (!cursor_.ParseString(key)) {
        return false;
      }
      cursor_.SkipWhitespace();
      if (!cursor_.Consume(':')) {
        return cursor_.Fail("expected ':' after JSON object key");
      }
      JsonValue value;
      if (!ParseValue(value)) {
        return false;
      }
      auto inserted = object.emplace(std::move(key), std::move(value));
      if (!inserted.second) {
        return cursor_.Fail("duplicate JSON object key");
      }
      cursor_.SkipWhitespace();
      if (cursor_.Consume('}')) {
        out = JsonValue::ObjectValue(std::move(object));
        return true;
      }
      if (!cursor_.Consume(',')) {
        return cursor_.Fail("expected ',' or '}' in JSON object");
      }
      cursor_.SkipWhitespace();
    }
  }

  bool ParseArray(JsonValue &out) {
    cursor_.Consume('[');
    JsonValue::Array array;
    cursor_.SkipWhitespace();
    if (cursor_.Consume(']')) {
      out = JsonValue::ArrayValue(std::move(array));
      return true;
    }
    while (true) {
      JsonValue value;
      if (!ParseValue(value)) {
        return false;
      }
      array.push_back(std::move(value));
      cursor_.SkipWhitespace();
      if (cursor_.Consume(']')) {
        out = JsonValue::ArrayValue(std::move(array));
        return true;
      }
      if (!cursor_.Consume(',')) {
        return cursor_.Fail("expected ',' or ']' in JSON array");
      }
      cursor_.SkipWhitespace();
    }
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
