#include "support/json_probe_writer.h"
#include "support/output_expectations.h"

#include <sstream>
#include <string>

int main() {
  using objc3c::runtime::probe::JsonEscapedString;
  using objc3c::runtime::probe::JsonEscape;
  using objc3c::runtime::probe::JsonFieldSeparator;
  using objc3c::runtime::probe::JsonString;
  using objc3c::runtime::probe::JsonStringOrNull;
  using objc3c::runtime::probe::ExpectTextEqual;
  using objc3c::runtime::probe::WriteJsonBoolField;
  using objc3c::runtime::probe::WriteJsonIntField;
  using objc3c::runtime::probe::WriteJsonStringField;
  using objc3c::runtime::probe::WriteJsonUInt64Field;

  if (ExpectTextEqual(JsonString(""), "\"\"", "empty JSON string", 1) != 0) {
    return 1;
  }
  if (ExpectTextEqual(JsonString("quote\"slash\\"), "\"quote\\\"slash\\\\\"",
                      "quote/backslash escaping", 2) != 0) {
    return 2;
  }
  if (ExpectTextEqual(JsonString("line\ncarriage\rtab\tback\bform\f"),
                      "\"line\\ncarriage\\rtab\\tback\\bform\\f\"",
                      "control escape aliases", 3) != 0) {
    return 3;
  }
  const std::string control_byte(1, static_cast<char>(0x01));
  if (ExpectTextEqual(JsonString(control_byte), "\"\\u0001\"",
                      "generic control escaping", 4) != 0) {
    return 4;
  }
  if (ExpectTextEqual(JsonString("\xC3\xA9"), "\"\xC3\xA9\"",
                      "non-ASCII byte preservation", 5) != 0) {
    return 5;
  }
  if (ExpectTextEqual(JsonEscape(nullptr), "", "null escape copy", 6) != 0) {
    return 6;
  }
  if (ExpectTextEqual(JsonStringOrNull(nullptr), "null",
                      "null JSON value", 7) != 0) {
    return 7;
  }

  std::ostringstream out;
  JsonFieldSeparator fields;
  out << '{';
  WriteJsonIntField(out, fields, "copy_status", -1);
  WriteJsonUInt64Field(out, fields, "count", 42);
  WriteJsonBoolField(out, fields, "ready", true);
  WriteJsonStringField(out, fields, "name", "objc3");
  WriteJsonStringField(out, fields, "missing", nullptr);
  out << '}';
  if (ExpectTextEqual(out.str(),
                      "{\"copy_status\":-1,\"count\":42,\"ready\":true,"
                      "\"name\":\"objc3\",\"missing\":null}",
                      "representative JSON object", 8) != 0) {
    return 8;
  }

  return 0;
}
