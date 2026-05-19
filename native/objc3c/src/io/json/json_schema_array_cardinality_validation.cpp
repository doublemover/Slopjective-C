#include "io/json/json_schema_array_cardinality_validation.h"

#include "io/json/json_schema_array_cardinality_max_items_validation.h"
#include "io/json/json_schema_array_cardinality_min_items_validation.h"

namespace objc3::io::json {

void ValidateJsonSchemaArrayCardinality(const JsonValue &schema,
                                        const JsonValue &payload,
                                        const std::string &instance_path,
                                        const std::string &schema_path,
                                        JsonSchemaResult &result) {
  ValidateJsonSchemaArrayMinItems(schema, payload, instance_path, schema_path,
                                  result);
  ValidateJsonSchemaArrayMaxItems(schema, payload, instance_path, schema_path,
                                  result);
}

}  // namespace objc3::io::json
