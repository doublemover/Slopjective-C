#include "io/json/json_value_writer.h"

#include <iomanip>

#include "io/json/json_value_container_writer.h"
#include "io/objc3_json.h"

namespace objc3::io::json {

void WriteJsonValue(std::ostream &out, const JsonValue &value) {
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
    case JsonValue::Kind::kArray:
      WriteJsonArrayValue(out, value.AsArray());
      return;
    case JsonValue::Kind::kObject:
      WriteJsonObjectValue(out, value.AsObject());
      return;
  }
}

}  // namespace objc3::io::json
