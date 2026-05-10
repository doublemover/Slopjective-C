#include "io/json/json_string_escape_sinks.h"

#include <ostream>

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

}  // namespace

JsonStringEscapeEmitter MakeJsonStringAppendEscapeEmitter(std::string &out) {
  return JsonStringEscapeEmitter{&out, AppendEscapedText, AppendEscapedChar};
}

JsonStringEscapeEmitter MakeJsonStringStreamEscapeEmitter(std::ostream &out) {
  return JsonStringEscapeEmitter{&out, WriteEscapedText, WriteEscapedChar};
}

}  // namespace objc3::io::json
