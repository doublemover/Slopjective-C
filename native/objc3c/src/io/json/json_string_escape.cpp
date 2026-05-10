#include "io/json/json_string_escape.h"

#include "io/json/json_string_escape_classification.h"

namespace objc3::io::json {
namespace {

template <typename EmitText, typename EmitChar>
void EmitEscapedJsonString(std::string_view value, EmitText emit_text,
                           EmitChar emit_char) {
  constexpr char kHex[] = "0123456789abcdef";
  for (const unsigned char c : value) {
    const JsonStringEscapeClassification escape =
        ClassifyJsonStringEscape(c);
    if (escape.short_escape != nullptr) {
      emit_text(escape.short_escape);
      continue;
    }
    if (escape.unicode_control_escape) {
      emit_text("\\u00");
      emit_char(kHex[(c >> 4u) & 0x0fu]);
      emit_char(kHex[c & 0x0fu]);
      continue;
    }
    emit_char(static_cast<char>(c));
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
