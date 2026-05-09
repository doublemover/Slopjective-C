#include "io/json/json_schema_validation.h"

#include "io/json/json_pointer.h"
#include "io/json/json_schema_array_validation.h"
#include "io/json/json_schema_composition_validation.h"
#include "io/json/json_schema_errors.h"
#include "io/json/json_schema_object_validation.h"
#include "io/json/json_schema_scalar_validation.h"
#include "io/json/json_schema_value_keyword_validation.h"

namespace objc3::io::json {

void ValidateJsonSchemaNode(const JsonValue &schema_root,
                            const JsonValue &schema,
                            const JsonValue &payload,
                            std::string instance_path,
                            std::string schema_path,
                            JsonSchemaResult &result) {
  if (!schema.IsObject()) {
    AddJsonSchemaContractError(result, "invalid_schema_node", schema_path,
                               "schema node must be a JSON object");
    return;
  }
  if (const JsonValue *ref = schema.Find("$ref"); ref != nullptr) {
    if (!ref->IsString()) {
      AddJsonSchemaContractError(
          result, "invalid_ref", JsonSchemaKeywordPath(schema_path, "$ref"),
          "$ref must be a local JSON pointer string");
      return;
    }
    const JsonValue *resolved =
        ResolveLocalJsonPointerRef(schema_root, ref->AsString());
    if (resolved == nullptr) {
      AddJsonSchemaContractError(
          result, "unresolved_ref", JsonSchemaKeywordPath(schema_path, "$ref"),
          "unresolved schema reference " + ref->AsString());
      return;
    }
    ValidateJsonSchemaNode(schema_root, *resolved, payload, instance_path,
                           ref->AsString(), result);
    return;
  }
  ValidateJsonSchemaCompositionKeywords(schema_root, schema, payload,
                                        instance_path, schema_path, result);
  if (!ValidateJsonSchemaValueKeywords(schema, payload, instance_path,
                                       schema_path, result)) {
    return;
  }
  const JsonValue *properties = schema.Find("properties");
  ValidateJsonSchemaObjectFields(schema_root, schema, payload, properties,
                                 instance_path, schema_path, result);
  ValidateJsonSchemaArrayFields(schema_root, schema, payload, instance_path,
                                schema_path, result);
  ValidateJsonSchemaScalarFields(schema, payload, instance_path, schema_path,
                                 result);
}

}  // namespace objc3::io::json
