#include "io/objc3_json.h"

#include "io/json/json_string_escape.h"

namespace objc3::io {

std::string EscapeJsonString(std::string_view value) {
  return json::EscapeJsonStringContent(value);
}

void WriteJsonString(std::ostream &out, std::string_view value) {
  out << '"';
  json::WriteJsonStringContent(out, value);
  out << '"';
}

}  // namespace objc3::io
