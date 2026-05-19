#include "io/json/json_value_container_writer.h"

#include <cstddef>

#include "io/json/json_value_writer.h"
#include "io/objc3_json.h"

namespace objc3::io::json {

void WriteJsonArrayValue(std::ostream &out, const JsonValue::Array &array) {
  out << '[';
  for (std::size_t i = 0; i < array.size(); ++i) {
    if (i > 0) {
      out << ',';
    }
    WriteJsonValue(out, array[i]);
  }
  out << ']';
}

void WriteJsonObjectValue(std::ostream &out, const JsonValue::Object &object) {
  out << '{';
  bool first = true;
  for (const auto &[key, item] : object) {
    if (!first) {
      out << ',';
    }
    first = false;
    objc3::io::WriteJsonString(out, key);
    out << ':';
    WriteJsonValue(out, item);
  }
  out << '}';
}

}  // namespace objc3::io::json
