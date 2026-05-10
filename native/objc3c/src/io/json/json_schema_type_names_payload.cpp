#include "io/json/json_schema_type_names.h"

namespace objc3::io::json {

bool JsonSchemaTypeNameMatchesPayload(std::string_view type,
                                      const JsonValue &payload) {
  if (type == "null") {
    return payload.IsNull();
  }
  if (type == "boolean") {
    return payload.IsBool();
  }
  if (type == "number" || type == "integer") {
    return payload.IsNumber();
  }
  if (type == "string") {
    return payload.IsString();
  }
  if (type == "array") {
    return payload.IsArray();
  }
  if (type == "object") {
    return payload.IsObject();
  }
  return false;
}

}  // namespace objc3::io::json
