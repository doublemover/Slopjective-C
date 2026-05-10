#include "io/json/json_string_escape.h"

#include <ostream>
#include <string>

#include "io/json/json_string_escape_emission.h"

namespace objc3::io::json {
namespace {

void AppendEscapedText(void *context, std::string_view text) {
  static_cast<std::string *>(context)->append(text);
}

void AppendEscapedChar(void *context, char ch) {
  static_cast<std::string *>(context)->push_back(ch);
}

void WriteEscapedText(void *context, std::string_view text) {
  *static_cast<std::ostream *>(context) << text;
}

void WriteEscapedChar(void *context, char ch) {
  *static_cast<std::ostream *>(context) << ch;
}

void EmitEscapedJsonString(std::string_view value,
                           const JsonStringEscapeEmitter &emitter) {
  for (const unsigned char c : value) {
    EmitEscapedJsonStringByte(c, emitter);
  }
}

}  // namespace

std::string EscapeJsonStringContent(std::string_view value) {
  std::string escaped;
  escaped.reserve(value.size());
  EmitEscapedJsonString(value, JsonStringEscapeEmitter{
                                   &escaped,
                                   AppendEscapedText,
                                   AppendEscapedChar,
                               });
  return escaped;
}

void WriteJsonStringContent(std::ostream &out, std::string_view value) {
  EmitEscapedJsonString(value, JsonStringEscapeEmitter{
                                   &out,
                                   WriteEscapedText,
                                   WriteEscapedChar,
                               });
}

}  // namespace objc3::io::json
