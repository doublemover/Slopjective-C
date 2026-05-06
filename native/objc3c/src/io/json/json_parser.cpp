#include "io/json/json_parser.h"

#include <cctype>
#include <cstdlib>
#include <string>
#include <utility>

namespace objc3::io::json {
namespace {

class Parser {
 public:
  explicit Parser(std::string_view text) : text_(text) {}

  JsonParseResult Parse() {
    JsonValue value;
    if (!ParseValue(value)) {
      return {JsonValue::Null(), error_};
    }
    SkipWhitespace();
    if (cursor_ != text_.size()) {
      Fail("unexpected trailing JSON content");
      return {JsonValue::Null(), error_};
    }
    return {std::move(value), std::nullopt};
  }

 private:
  bool ParseValue(JsonValue &out) {
    SkipWhitespace();
    if (cursor_ >= text_.size()) {
      return Fail("unexpected end of JSON input");
    }
    const char ch = text_[cursor_];
    if (ch == '{') {
      return ParseObject(out);
    }
    if (ch == '[') {
      return ParseArray(out);
    }
    if (ch == '"') {
      std::string value;
      if (!ParseString(value)) {
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
    if (ch == '-' || std::isdigit(static_cast<unsigned char>(ch)) != 0) {
      return ParseNumber(out);
    }
    return Fail("unexpected JSON token");
  }

  bool ParseObject(JsonValue &out) {
    ++cursor_;
    JsonValue::Object object;
    SkipWhitespace();
    if (Consume('}')) {
      out = JsonValue::ObjectValue(std::move(object));
      return true;
    }
    while (true) {
      std::string key;
      if (!ParseString(key)) {
        return false;
      }
      SkipWhitespace();
      if (!Consume(':')) {
        return Fail("expected ':' after JSON object key");
      }
      JsonValue value;
      if (!ParseValue(value)) {
        return false;
      }
      auto inserted = object.emplace(std::move(key), std::move(value));
      if (!inserted.second) {
        return Fail("duplicate JSON object key");
      }
      SkipWhitespace();
      if (Consume('}')) {
        out = JsonValue::ObjectValue(std::move(object));
        return true;
      }
      if (!Consume(',')) {
        return Fail("expected ',' or '}' in JSON object");
      }
      SkipWhitespace();
    }
  }

  bool ParseArray(JsonValue &out) {
    ++cursor_;
    JsonValue::Array array;
    SkipWhitespace();
    if (Consume(']')) {
      out = JsonValue::ArrayValue(std::move(array));
      return true;
    }
    while (true) {
      JsonValue value;
      if (!ParseValue(value)) {
        return false;
      }
      array.push_back(std::move(value));
      SkipWhitespace();
      if (Consume(']')) {
        out = JsonValue::ArrayValue(std::move(array));
        return true;
      }
      if (!Consume(',')) {
        return Fail("expected ',' or ']' in JSON array");
      }
      SkipWhitespace();
    }
  }

  bool ParseString(std::string &out) {
    if (!Consume('"')) {
      return Fail("expected JSON string");
    }
    out.clear();
    while (cursor_ < text_.size()) {
      const unsigned char ch = static_cast<unsigned char>(text_[cursor_++]);
      if (ch == '"') {
        return true;
      }
      if (ch < 0x20u) {
        return Fail("unescaped control character in JSON string");
      }
      if (ch != '\\') {
        out.push_back(static_cast<char>(ch));
        continue;
      }
      if (cursor_ >= text_.size()) {
        return Fail("unterminated JSON string escape");
      }
      const char escaped = text_[cursor_++];
      switch (escaped) {
        case '"':
        case '\\':
        case '/':
          out.push_back(escaped);
          break;
        case 'b':
          out.push_back('\b');
          break;
        case 'f':
          out.push_back('\f');
          break;
        case 'n':
          out.push_back('\n');
          break;
        case 'r':
          out.push_back('\r');
          break;
        case 't':
          out.push_back('\t');
          break;
        case 'u':
          if (!ParseUnicodeEscape(out)) {
            return false;
          }
          break;
        default:
          return Fail("invalid JSON string escape");
      }
    }
    return Fail("unterminated JSON string");
  }

  bool ParseUnicodeEscape(std::string &out) {
    if (cursor_ + 4 > text_.size()) {
      return Fail("short JSON unicode escape");
    }
    unsigned codepoint = 0;
    for (int i = 0; i < 4; ++i) {
      const char ch = text_[cursor_++];
      codepoint <<= 4u;
      if (ch >= '0' && ch <= '9') {
        codepoint += static_cast<unsigned>(ch - '0');
      } else if (ch >= 'a' && ch <= 'f') {
        codepoint += static_cast<unsigned>(10 + ch - 'a');
      } else if (ch >= 'A' && ch <= 'F') {
        codepoint += static_cast<unsigned>(10 + ch - 'A');
      } else {
        return Fail("invalid JSON unicode escape");
      }
    }
    if (codepoint <= 0x7fu) {
      out.push_back(static_cast<char>(codepoint));
      return true;
    }
    if (codepoint <= 0x7ffu) {
      out.push_back(static_cast<char>(0xc0u | (codepoint >> 6u)));
      out.push_back(static_cast<char>(0x80u | (codepoint & 0x3fu)));
      return true;
    }
    out.push_back(static_cast<char>(0xe0u | (codepoint >> 12u)));
    out.push_back(static_cast<char>(0x80u | ((codepoint >> 6u) & 0x3fu)));
    out.push_back(static_cast<char>(0x80u | (codepoint & 0x3fu)));
    return true;
  }

  bool ParseNumber(JsonValue &out) {
    const std::size_t start = cursor_;
    if (Consume('-') && cursor_ >= text_.size()) {
      return Fail("incomplete JSON number");
    }
    if (Consume('0')) {
      if (cursor_ < text_.size() && std::isdigit(static_cast<unsigned char>(text_[cursor_])) != 0) {
        return Fail("JSON number has leading zero");
      }
    } else if (!ConsumeDigits()) {
      return Fail("expected JSON number digits");
    }
    if (Consume('.')) {
      if (!ConsumeDigits()) {
        return Fail("expected JSON number fraction digits");
      }
    }
    if (cursor_ < text_.size() && (text_[cursor_] == 'e' || text_[cursor_] == 'E')) {
      ++cursor_;
      if (cursor_ < text_.size() && (text_[cursor_] == '+' || text_[cursor_] == '-')) {
        ++cursor_;
      }
      if (!ConsumeDigits()) {
        return Fail("expected JSON number exponent digits");
      }
    }
    const std::string raw(text_.substr(start, cursor_ - start));
    char *end = nullptr;
    const double parsed = std::strtod(raw.c_str(), &end);
    if (end == nullptr || *end != '\0') {
      return Fail("invalid JSON number");
    }
    out = JsonValue::Number(parsed);
    return true;
  }

  bool ParseLiteral(std::string_view literal, JsonValue value, JsonValue &out) {
    if (text_.substr(cursor_, literal.size()) != literal) {
      return Fail("invalid JSON literal");
    }
    cursor_ += literal.size();
    out = std::move(value);
    return true;
  }

  bool ConsumeDigits() {
    const std::size_t start = cursor_;
    while (cursor_ < text_.size() && std::isdigit(static_cast<unsigned char>(text_[cursor_])) != 0) {
      ++cursor_;
    }
    return cursor_ > start;
  }

  bool Consume(char expected) {
    if (cursor_ < text_.size() && text_[cursor_] == expected) {
      ++cursor_;
      return true;
    }
    return false;
  }

  void SkipWhitespace() {
    while (cursor_ < text_.size()) {
      const char ch = text_[cursor_];
      if (ch != ' ' && ch != '\t' && ch != '\r' && ch != '\n') {
        return;
      }
      ++cursor_;
    }
  }

  bool Fail(std::string message) {
    error_ = JsonError{std::move(message), cursor_};
    return false;
  }

  std::string_view text_;
  std::size_t cursor_ = 0;
  std::optional<JsonError> error_;
};

}  // namespace

JsonParseResult ParseJson(std::string_view text) {
  return Parser(text).Parse();
}

}  // namespace objc3::io::json
