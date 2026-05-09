#include "io/json/json_schema_validation.h"

#include "io/json/json_schema_composition_validation.h"
#include "io/json/json_schema_node_preflight_validation.h"
#include "io/json/json_schema_runtime_field_validation.h"
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
  ValidateJsonSchemaRuntimeFields(schema_root, schema, payload, instance_path,
                                  schema_path, result);
}

}  // namespace objc3::io::json
