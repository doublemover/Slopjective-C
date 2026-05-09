#include "ir/objc3_ir_literal_parsing.h"

#include <cctype>

#include "parse/objc3_parse_support.h"

bool ParseOwnershipResourceInvalidLiteral(const std::string &text, int &value) {
  std::string normalized;
  normalized.reserve(text.size());
  for (unsigned char ch : text) {
    if (!std::isspace(ch)) {
      normalized.push_back(static_cast<char>(ch));
    }
  }
  if (normalized.empty()) {
    return false;
  }
  bool negative = false;
  if (normalized.front() == '+' || normalized.front() == '-') {
    negative = normalized.front() == '-';
    normalized.erase(normalized.begin());
  }
  if (normalized.empty() ||
      !objc3c::parse::support::ParseIntegerLiteralValue(normalized, value)) {
    return false;
  }
  if (negative) {
    value = -value;
  }
  return true;
}
