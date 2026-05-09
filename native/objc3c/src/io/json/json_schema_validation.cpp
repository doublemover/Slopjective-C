#include "io/json/json_schema_validation.h"

#include "io/json/json_schema_array_validation.h"
#include "io/json/json_schema_composition_validation.h"
#include "io/json/json_schema_node_preflight_validation.h"
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
  if (!ValidateJsonSchemaNodePreflight(schema_root, schema, payload,
                                       instance_path, schema_path, result)) {
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
