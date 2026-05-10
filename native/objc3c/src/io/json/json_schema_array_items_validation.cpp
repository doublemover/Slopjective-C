#include "io/json/json_schema_array_items_validation.h"

#include "io/json/json_schema_array_items_element_validation.h"
#include "io/json/json_schema_array_items_keyword_validation.h"

namespace objc3::io::json {

bool ValidateJsonSchemaArrayItems(const JsonValue &schema_root,
                                  const JsonValue &schema,
                                  const JsonValue &payload,
                                  const std::string &instance_path,
                                  const std::string &schema_path,
                                  JsonSchemaResult &result) {
  const JsonValue *items = schema.Find("items");
  if (items != nullptr && payload.IsArray()) {
    if (!ValidateJsonSchemaArrayItemsKeyword(*items, schema_path, result)) {
      return false;
    }
    ValidateJsonSchemaArrayItemElements(schema_root, *items, payload,
                                        instance_path, schema_path, result);
  }
  return true;
}

}  // namespace objc3::io::json
