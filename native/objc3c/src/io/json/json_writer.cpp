#include "io/json/json_writer.h"

#include <iomanip>
#include <sstream>

#include "io/objc3_json.h"

namespace objc3::io::json {
namespace {

void WriteValue(std::ostream &out, const JsonValue &value) {
  switch (value.kind()) {
    case JsonValue::Kind::kNull:
      out << "null";
      return;
    case JsonValue::Kind::kBool:
      out << (value.AsBool() ? "true" : "false");
      return;
    case JsonValue::Kind::kNumber:
      out << std::setprecision(17) << value.AsNumber();
      return;
    case JsonValue::Kind::kString:
      objc3::io::WriteJsonString(out, value.AsString());
      return;
    case JsonValue::Kind::kArray: {
      out << '[';
      const auto &array = value.AsArray();
      for (std::size_t i = 0; i < array.size(); ++i) {
        if (i > 0) {
          out << ',';
        }
        WriteValue(out, array[i]);
      }
      out << ']';
      return;
    }
    case JsonValue::Kind::kObject: {
      out << '{';
      bool first = true;
      for (const auto &[key, item] : value.AsObject()) {
        if (!first) {
          out << ',';
        }
        first = false;
        objc3::io::WriteJsonString(out, key);
        out << ':';
        WriteValue(out, item);
      }
      out << '}';
      return;
    }
  }
}

}  // namespace

void WriteJson(std::ostream &out, const JsonValue &value) {
  WriteValue(out, value);
}

std::string RenderJson(const JsonValue &value) {
  std::ostringstream out;
  WriteJson(out, value);
  return out.str();
}

}  // namespace objc3::io::json
