#include "pipeline/runtime_import_json_helpers.h"

#include <cctype>
#include <exception>
#include <sstream>
#include <utility>

namespace objc3c::pipeline {

bool RuntimeImportJsonParser::AtEnd() const {
  return offset_ >= input_.size();
}

char RuntimeImportJsonParser::Peek() const {
  return AtEnd() ? '\0' : input_[offset_];
}

char RuntimeImportJsonParser::Consume() {
  return AtEnd() ? '\0' : input_[offset_++];
}

void RuntimeImportJsonParser::SkipWhitespace() {
  while (!AtEnd() &&
         std::isspace(static_cast<unsigned char>(input_[offset_])) != 0) {
    ++offset_;
  }
}

bool RuntimeImportJsonParser::ParseValue(RuntimeImportJsonValue &value,
                                         std::string &error) {
  SkipWhitespace();
  if (AtEnd()) {
    error = "unexpected end of JSON input";
    return false;
  }
  switch (Peek()) {
    case '{':
      return ParseObject(value, error);
    case '[':
      return ParseArray(value, error);
    case '"': {
      std::string string_value;
      if (!ParseString(string_value, error)) {
        return false;
      }
      value.value = std::move(string_value);
      return true;
    }
    case 't':
      if (!ConsumeKeyword("true")) {
        error = "invalid JSON literal";
        return false;
      }
      value.value = true;
      return true;
    case 'f':
      if (!ConsumeKeyword("false")) {
        error = "invalid JSON literal";
        return false;
      }
      value.value = false;
      return true;
    case 'n':
      if (!ConsumeKeyword("null")) {
        error = "invalid JSON literal";
        return false;
      }
      value.value = std::monostate{};
      return true;
    default:
      if (Peek() == '-' ||
          std::isdigit(static_cast<unsigned char>(Peek())) != 0) {
        std::int64_t number_value = 0;
        if (!ParseInteger(number_value, error)) {
          return false;
        }
        value.value = number_value;
        return true;
      }
      error = "unsupported JSON token";
      return false;
  }
}

bool RuntimeImportJsonParser::ParseObject(RuntimeImportJsonValue &value,
                                          std::string &error) {
  if (Consume() != '{') {
    error = "expected '{'";
    return false;
  }
  RuntimeImportJsonValue::Object object;
  SkipWhitespace();
  if (Peek() == '}') {
    Consume();
    value.value = std::move(object);
    return true;
  }
  while (true) {
    SkipWhitespace();
    std::string key;
    if (!ParseString(key, error)) {
      return false;
    }
    SkipWhitespace();
    if (Consume() != ':') {
      error = "expected ':' in JSON object";
      return false;
    }
    RuntimeImportJsonValue member;
    if (!ParseValue(member, error)) {
      return false;
    }
    object.emplace(std::move(key), std::move(member));
    SkipWhitespace();
    const char separator = Consume();
    if (separator == '}') {
      break;
    }
    if (separator != ',') {
      error = "expected ',' or '}' in JSON object";
      return false;
    }
  }
  value.value = std::move(object);
  return true;
}

bool RuntimeImportJsonParser::ParseArray(RuntimeImportJsonValue &value,
                                         std::string &error) {
  if (Consume() != '[') {
    error = "expected '['";
    return false;
  }
  RuntimeImportJsonValue::Array array;
  SkipWhitespace();
  if (Peek() == ']') {
    Consume();
    value.value = std::move(array);
    return true;
  }
  while (true) {
    RuntimeImportJsonValue element;
    if (!ParseValue(element, error)) {
      return false;
    }
    array.push_back(std::move(element));
    SkipWhitespace();
    const char separator = Consume();
    if (separator == ']') {
      break;
    }
    if (separator != ',') {
      error = "expected ',' or ']' in JSON array";
      return false;
    }
  }
  value.value = std::move(array);
  return true;
}

bool RuntimeImportJsonParser::ParseString(std::string &value,
                                          std::string &error) {
  if (Consume() != '"') {
    error = "expected string literal";
    return false;
  }
  std::ostringstream out;
  while (!AtEnd()) {
    const char ch = Consume();
    if (ch == '"') {
      value = out.str();
      return true;
    }
    if (ch == '\\') {
      if (AtEnd()) {
        error = "unterminated JSON escape sequence";
        return false;
      }
      const char escaped = Consume();
      switch (escaped) {
        case '"':
        case '\\':
        case '/':
          out << escaped;
          break;
        case 'b':
          out << '\b';
          break;
        case 'f':
          out << '\f';
          break;
        case 'n':
          out << '\n';
          break;
        case 'r':
          out << '\r';
          break;
        case 't':
          out << '\t';
          break;
        default:
          error = "unsupported JSON escape sequence";
          return false;
      }
    } else {
      out << ch;
    }
  }
  error = "unterminated JSON string literal";
  return false;
}

bool RuntimeImportJsonParser::ParseInteger(std::int64_t &value,
                                           std::string &error) {
  const std::size_t start = offset_;
  if (Peek() == '-') {
    Consume();
  }
  if (AtEnd() || std::isdigit(static_cast<unsigned char>(Peek())) == 0) {
    error = "invalid JSON number";
    return false;
  }
  if (Peek() == '0') {
    Consume();
  } else {
    while (!AtEnd() &&
           std::isdigit(static_cast<unsigned char>(Peek())) != 0) {
      Consume();
    }
  }
  if (!AtEnd() && (Peek() == '.' || Peek() == 'e' || Peek() == 'E')) {
    error = "floating-point JSON numbers are unsupported";
    return false;
  }
  const std::string token = input_.substr(start, offset_ - start);
  try {
    value = std::stoll(token);
  } catch (const std::exception &) {
    error = "invalid JSON integer range";
    return false;
  }
  return true;
}

bool RuntimeImportJsonParser::ConsumeKeyword(const char *keyword) {
  const std::size_t start = offset_;
  while (*keyword != '\0') {
    if (AtEnd() || Consume() != *keyword) {
      offset_ = start;
      return false;
    }
    ++keyword;
  }
  return true;
}

}  // namespace objc3c::pipeline
