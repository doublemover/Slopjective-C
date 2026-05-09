#include "io/json/json_schema_type.h"

#include "io/json/json_schema_type_names.h"

#include <sstream>

namespace objc3::io::json {

std::string JsonSchemaValueTypeName(const JsonValue &value) {
  return JsonSchemaValueKindName(value.kind());
}

std::string DescribeExpectedJsonSchemaType(const JsonValue &schema_type) {
  if (schema_type.IsString()) {
    return schema_type.AsString();
  }
  if (!schema_type.IsArray()) {
    return "any";
  }
  std::ostringstream out;
  bool first = true;
  for (const JsonValue &entry : schema_type.AsArray()) {
    if (!entry.IsString()) {
      continue;
    }
    if (!first) {
      out << "|";
    }
    first = false;
    out << entry.AsString();
  }
  return first ? "any" : out.str();
}

bool JsonSchemaMatchesType(const JsonValue &schema_type,
                           const JsonValue &payload) {
  if (schema_type.IsString()) {
    return JsonSchemaTypeNameMatchesPayload(schema_type.AsString(), payload);
  }
  if (!schema_type.IsArray()) {
    return true;
  }
  for (const JsonValue &entry : schema_type.AsArray()) {
    if (entry.IsString() &&
        JsonSchemaTypeNameMatchesPayload(entry.AsString(), payload)) {
      return true;
    }
  }
  return false;
}

}  // namespace objc3::io::json
