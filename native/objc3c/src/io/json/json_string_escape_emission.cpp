#include "io/json/json_string_escape_emission.h"

#include "io/json/json_string_escape_classification.h"

namespace objc3::io::json {

void EmitEscapedJsonStringByte(unsigned char value,
                               const JsonStringEscapeEmitter &emitter) {
  constexpr char kHex[] = "0123456789abcdef";
  const JsonStringEscapeClassification escape =
      ClassifyJsonStringEscape(value);
  if (escape.short_escape != nullptr) {
    emitter.emit_text(emitter.context, escape.short_escape);
    return;
  }
  if (escape.unicode_control_escape) {
    emitter.emit_text(emitter.context, "\\u00");
    emitter.emit_char(emitter.context, kHex[(value >> 4u) & 0x0fu]);
    emitter.emit_char(emitter.context, kHex[value & 0x0fu]);
    return;
  }
  emitter.emit_char(emitter.context, static_cast<char>(value));
}

}  // namespace objc3::io::json
