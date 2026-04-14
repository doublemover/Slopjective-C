#include "support/json_probe_writer.h"

#include <sstream>
#include <string>

namespace {

int ExpectEqual(const std::string &actual, const std::string &expected) {
  return actual == expected ? 0 : 1;
}

}  // namespace

int main() {
  using objc3c::runtime::probe::JsonEscapedString;
  using objc3c::runtime::probe::JsonEscape;
  using objc3c::runtime::probe::JsonFieldSeparator;
  using objc3c::runtime::probe::JsonString;
  using objc3c::runtime::probe::JsonStringOrNull;
  using objc3c::runtime::probe::WriteJsonBoolField;
  using objc3c::runtime::probe::WriteJsonIntField;
  using objc3c::runtime::probe::WriteJsonStringField;
  using objc3c::runtime::probe::WriteJsonUInt64Field;

  if (ExpectEqual(JsonString(""), "\"\"") != 0) {
    return 1;
  }
  if (ExpectEqual(JsonString("quote\"slash\\"),
                  "\"quote\\\"slash\\\\\"") != 0) {
    return 2;
  }
  if (ExpectEqual(JsonString("line\ncarriage\rtab\tback\bform\f"),
                  "\"line\\ncarriage\\rtab\\tback\\bform\\f\"") != 0) {
    return 3;
  }
  const std::string control_byte(1, static_cast<char>(0x01));
  if (ExpectEqual(JsonString(control_byte), "\"\\u0001\"") != 0) {
    return 4;
  }
  if (ExpectEqual(JsonString("\xC3\xA9"), "\"\xC3\xA9\"") != 0) {
    return 5;
  }
  if (ExpectEqual(JsonEscape(nullptr), "") != 0) {
    return 6;
  }
  if (ExpectEqual(JsonStringOrNull(nullptr), "null") != 0) {
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
  if (ExpectEqual(out.str(),
                  "{\"copy_status\":-1,\"count\":42,\"ready\":true,"
                  "\"name\":\"objc3\",\"missing\":null}") != 0) {
    return 8;
  }

  return 0;
}
