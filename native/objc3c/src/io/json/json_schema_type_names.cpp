#include "io/json/json_schema_type_names.h"

namespace objc3::io::json {

std::string JsonSchemaValueKindName(JsonValue::Kind kind) {
  switch (kind) {
    case JsonValue::Kind::kNull:
      return "null";
    case JsonValue::Kind::kBool:
      return "boolean";
    case JsonValue::Kind::kNumber:
      return "number";
    case JsonValue::Kind::kString:
      return "string";
    case JsonValue::Kind::kArray:
      return "array";
    case JsonValue::Kind::kObject:
      return "object";
  }
  return "unknown";
}

}  // namespace objc3::io::json
