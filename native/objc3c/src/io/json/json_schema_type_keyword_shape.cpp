#include "io/json/json_schema_type_keyword_shape.h"

namespace objc3::io::json {

bool JsonSchemaTypeKeywordAllowsAny(const JsonValue &schema_type) {
  return !schema_type.IsString() && !schema_type.IsArray();
}

std::vector<std::string_view> JsonSchemaTypeKeywordNames(
    const JsonValue &schema_type) {
  std::vector<std::string_view> names;
  if (schema_type.IsString()) {
    names.push_back(schema_type.AsString());
    return names;
  }
  if (!schema_type.IsArray()) {
    return names;
  }
  for (const JsonValue &entry : schema_type.AsArray()) {
    if (entry.IsString()) {
      names.push_back(entry.AsString());
    }
  }
  return names;
}

}  // namespace objc3::io::json
