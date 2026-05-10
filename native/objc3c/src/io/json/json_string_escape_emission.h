#pragma once

#include <string_view>

namespace objc3::io::json {

struct JsonStringEscapeEmitter {
  void *context;
  void (*emit_text)(void *context, std::string_view text);
  void (*emit_char)(void *context, char ch);
};

void EmitEscapedJsonStringByte(unsigned char value,
                               const JsonStringEscapeEmitter &emitter);

}  // namespace objc3::io::json
