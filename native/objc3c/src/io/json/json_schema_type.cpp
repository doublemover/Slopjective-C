#include "io/json/json_schema_type.h"

#include "io/json/json_schema_type_keyword_shape.h"
#include "io/json/json_schema_type_names.h"

#include <sstream>
#include <string_view>

namespace objc3::io::json {

std::string JsonSchemaValueTypeName(const JsonValue &value) {
  return JsonSchemaValueKindName(value.kind());
}

std::string DescribeExpectedJsonSchemaType(const JsonValue &schema_type) {
  if (JsonSchemaTypeKeywordAllowsAny(schema_type)) {
    return "any";
  }
  std::ostringstream out;
  bool first = true;
  for (std::string_view type : JsonSchemaTypeKeywordNames(schema_type)) {
    if (!first) {
      out << "|";
    }
    first = false;
    out << type;
  }
  return first ? "any" : out.str();
}

bool JsonSchemaMatchesType(const JsonValue &schema_type,
                           const JsonValue &payload) {
  if (JsonSchemaTypeKeywordAllowsAny(schema_type)) {
    return true;
  }
  for (std::string_view type : JsonSchemaTypeKeywordNames(schema_type)) {
    if (JsonSchemaTypeNameMatchesPayload(type, payload)) {
      return true;
    }
  }
  return false;
}

}  // namespace objc3::io::json
