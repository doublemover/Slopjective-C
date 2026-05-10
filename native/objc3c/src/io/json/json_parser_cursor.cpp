#include "io/json/json_parser_cursor.h"

#include <cctype>
#include <cstdlib>
#include <string>
#include <utility>

namespace objc3::io::json {

JsonParserCursor::JsonParserCursor(std::string_view text) : text_(text) {}

bool JsonParserCursor::IsDigit(char ch) {
  return std::isdigit(static_cast<unsigned char>(ch)) != 0;
}

bool JsonParserCursor::AtEnd() const {
  return cursor_ >= text_.size();
}

char JsonParserCursor::Peek() const {
  return text_[cursor_];
}

const std::optional<JsonError> &JsonParserCursor::error() const {
  return error_;
}

void JsonParserCursor::SkipWhitespace() {
  while (cursor_ < text_.size()) {
    const char ch = text_[cursor_];
    if (ch != ' ' && ch != '\t' && ch != '\r' && ch != '\n') {
      return;
    }
    ++cursor_;
  }
}

bool JsonParserCursor::Consume(char expected) {
  if (cursor_ < text_.size() && text_[cursor_] == expected) {
    ++cursor_;
    return true;
  }
  return false;
}

bool JsonParserCursor::ConsumeLiteral(std::string_view literal) {
  if (text_.substr(cursor_, literal.size()) != literal) {
    return false;
  }
  cursor_ += literal.size();
  return true;
}

bool JsonParserCursor::ParseString(std::string &out) {
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

bool JsonParserCursor::ParseNumber(JsonValue &out) {
  const std::size_t start = cursor_;
  if (Consume('-') && cursor_ >= text_.size()) {
    return Fail("incomplete JSON number");
  }
  if (Consume('0')) {
    if (cursor_ < text_.size() && IsDigit(text_[cursor_])) {
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
  if (cursor_ < text_.size() &&
      (text_[cursor_] == 'e' || text_[cursor_] == 'E')) {
    ++cursor_;
    if (cursor_ < text_.size() &&
        (text_[cursor_] == '+' || text_[cursor_] == '-')) {
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

bool JsonParserCursor::Fail(std::string message) {
  error_ = JsonError{std::move(message), cursor_};
  return false;
}

bool JsonParserCursor::ConsumeDigits() {
  const std::size_t start = cursor_;
  while (cursor_ < text_.size() && IsDigit(text_[cursor_])) {
    ++cursor_;
  }
  return cursor_ > start;
}

bool JsonParserCursor::ParseUnicodeEscape(std::string &out) {
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

}  // namespace objc3::io::json
