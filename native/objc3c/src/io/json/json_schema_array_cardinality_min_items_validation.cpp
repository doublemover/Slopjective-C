#include "io/json/json_schema_array_cardinality_min_items_validation.h"

#include "io/json/json_schema_array_cardinality_min_items_keyword_validation.h"
#include "io/json/json_schema_array_cardinality_min_items_payload_validation.h"

namespace objc3::io::json {

void ValidateJsonSchemaArrayMinItems(const JsonValue &schema,
                                     const JsonValue &payload,
                                     const std::string &instance_path,
                                     const std::string &schema_path,
                                     JsonSchemaResult &result) {
  const JsonValue *min_items = ValidateJsonSchemaArrayMinItemsKeyword(
      schema, payload, schema_path, result);
  if (min_items == nullptr) {
    return;
  }
  ValidateJsonSchemaArrayMinItemsPayload(*min_items, payload, instance_path,
                                         schema_path, result);
}

}  // namespace objc3::io::json
