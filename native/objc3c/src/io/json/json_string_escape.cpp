#include "io/json/json_string_escape.h"

namespace objc3::io::json {
namespace {

template <typename EmitText, typename EmitChar>
void EmitEscapedJsonString(std::string_view value, EmitText emit_text,
                           EmitChar emit_char) {
  constexpr char kHex[] = "0123456789abcdef";
  for (const unsigned char c : value) {
    switch (c) {
      case '"':
        emit_text("\\\"");
        break;
      case '\\':
        emit_text("\\\\");
        break;
      case '\b':
        emit_text("\\b");
        break;
      case '\f':
        emit_text("\\f");
        break;
      case '\n':
        emit_text("\\n");
        break;
      case '\r':
        emit_text("\\r");
        break;
      case '\t':
        emit_text("\\t");
        break;
      default:
        if (c < 0x20u) {
          emit_text("\\u00");
          emit_char(kHex[(c >> 4u) & 0x0fu]);
          emit_char(kHex[c & 0x0fu]);
        } else {
          emit_char(static_cast<char>(c));
        }
        break;
    }
  }
}

}  // namespace

std::string EscapeJsonStringContent(std::string_view value) {
  std::string escaped;
  escaped.reserve(value.size());
  EmitEscapedJsonString(
      value,
      [&escaped](std::string_view text) { escaped.append(text); },
      [&escaped](char ch) { escaped.push_back(ch); });
  return escaped;
}

void WriteJsonStringContent(std::ostream &out, std::string_view value) {
  EmitEscapedJsonString(
      value,
      [&out](std::string_view text) { out << text; },
      [&out](char ch) { out << ch; });
}

}  // namespace objc3::io::json

