#include "io/json/json_schema_validation.h"

#include "io/json/json_equivalence.h"
#include "io/json/json_pointer.h"
#include "io/json/json_schema_array_validation.h"
#include "io/json/json_schema_errors.h"
#include "io/json/json_schema_object_validation.h"
#include "io/json/json_schema_scalar_validation.h"
#include "io/json/json_schema_subschema.h"
#include "io/json/json_schema_type.h"

#include <sstream>

namespace objc3::io::json {

void ValidateJsonSchemaNode(const JsonValue &schema_root,
                            const JsonValue &schema,
                            const JsonValue &payload,
                            std::string path,
                            JsonSchemaResult &result) {
  if (!schema.IsObject()) {
    return;
  }
  if (const auto ref = schema.GetString("$ref")) {
    const JsonValue *resolved = ResolveLocalJsonPointerRef(schema_root, *ref);
    if (resolved == nullptr) {
      AddJsonSchemaError(result, path + " unresolved schema reference " + *ref);
      return;
    }
    ValidateJsonSchemaNode(schema_root, *resolved, payload, path, result);
    return;
  }
  const JsonValue *all_of = schema.Find("allOf");
  if (all_of != nullptr && all_of->IsArray()) {
    for (const JsonValue &candidate : all_of->AsArray()) {
      ValidateJsonSchemaNode(schema_root, candidate, payload, path, result);
    }
  }
  const JsonValue *any_of = schema.Find("anyOf");
  if (any_of != nullptr && any_of->IsArray()) {
    bool matched = false;
    for (const JsonValue &candidate : any_of->AsArray()) {
      if (JsonSubschemaPasses(schema_root, candidate, payload, path)) {
        matched = true;
        break;
      }
    }
    if (!matched) {
      AddJsonSchemaError(result, path + " did not match any allowed schema");
    }
  }
  if (const JsonValue *schema_type = schema.Find("type");
      schema_type != nullptr) {
    if (!JsonSchemaMatchesType(*schema_type, payload)) {
      std::ostringstream out;
      out << path << " expected "
          << DescribeExpectedJsonSchemaType(*schema_type) << " but found "
          << JsonSchemaValueTypeName(payload);
      AddJsonSchemaError(result, out.str());
      return;
    }
  }
  const JsonValue *const_value = schema.Find("const");
  if (const_value != nullptr && !JsonEquals(*const_value, payload)) {
    AddJsonSchemaError(result, path + " did not match const value");
  }
  const JsonValue *enum_values = schema.Find("enum");
  if (enum_values != nullptr && enum_values->IsArray()) {
    bool matched = false;
    for (const JsonValue &candidate : enum_values->AsArray()) {
      if (JsonEquals(candidate, payload)) {
        matched = true;
        break;
      }
    }
    if (!matched) {
      AddJsonSchemaError(result, path + " did not match enum values");
    }
  }
  const JsonValue *properties = schema.Find("properties");
  ValidateJsonSchemaObjectFields(schema_root, schema, payload, properties, path,
                                 result);
  ValidateJsonSchemaArrayFields(schema_root, schema, payload, path, result);
  ValidateJsonSchemaScalarFields(schema, payload, path, result);
}

}  // namespace objc3::io::json
