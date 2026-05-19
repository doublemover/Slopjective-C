#include "io/json/json_schema_array_cardinality_max_items_validation.h"

#include "io/json/json_schema_array_cardinality_max_items_keyword_validation.h"
#include "io/json/json_schema_array_cardinality_max_items_payload_validation.h"

namespace objc3::io::json {

void ValidateJsonSchemaArrayMaxItems(const JsonValue &schema,
                                     const JsonValue &payload,
                                     const std::string &instance_path,
                                     const std::string &schema_path,
                                     JsonSchemaResult &result) {
  const JsonValue *max_items = ValidateJsonSchemaArrayMaxItemsKeyword(
      schema, payload, schema_path, result);
  if (max_items == nullptr) {
    return;
  }
  ValidateJsonSchemaArrayMaxItemsPayload(*max_items, payload, instance_path,
                                         schema_path, result);
}

}  // namespace objc3::io::json
