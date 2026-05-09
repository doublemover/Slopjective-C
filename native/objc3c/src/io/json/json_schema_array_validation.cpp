#include "io/json/json_schema_array_validation.h"

#include "io/json/json_schema_array_cardinality_validation.h"
#include "io/json/json_schema_array_contains_validation.h"
#include "io/json/json_schema_array_items_validation.h"
#include "io/json/json_schema_array_uniqueness_validation.h"

namespace objc3::io::json {

void ValidateJsonSchemaArrayFields(const JsonValue &schema_root,
                                   const JsonValue &schema,
                                   const JsonValue &payload,
                                   const std::string &instance_path,
                                   const std::string &schema_path,
                                   JsonSchemaResult &result) {
  if (!ValidateJsonSchemaArrayItems(schema_root, schema, payload, instance_path,
                                    schema_path, result)) {
    return;
  }
  if (!ValidateJsonSchemaArrayContains(schema_root, schema, payload,
                                       instance_path, schema_path, result)) {
    return;
  }
  ValidateJsonSchemaArrayCardinality(schema, payload, instance_path,
                                     schema_path, result);
  if (!ValidateJsonSchemaArrayUniqueness(schema, payload, instance_path,
                                         schema_path, result)) {
    return;
  }
}

}  // namespace objc3::io::json
