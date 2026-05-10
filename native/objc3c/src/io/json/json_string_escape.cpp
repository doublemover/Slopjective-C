#include "io/json/json_string_escape.h"

#include <ostream>
#include <string>

#include "io/json/json_string_escape_emission.h"
#include "io/json/json_string_escape_sinks.h"

namespace objc3::io::json {
namespace {

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
  EmitEscapedJsonString(value, MakeJsonStringAppendEscapeEmitter(escaped));
  return escaped;
}

void WriteJsonStringContent(std::ostream &out, std::string_view value) {
  EmitEscapedJsonString(value, MakeJsonStringStreamEscapeEmitter(out));
}

}  // namespace objc3::io::json
