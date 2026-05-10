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
  const JsonSchemaArrayItemsKeywordValidation items =
      ValidateJsonSchemaArrayItemsKeyword(schema, payload, schema_path,
                                          result);
  if (!items.valid) {
    return false;
  }
  if (items.value == nullptr) {
    return true;
  }
  ValidateJsonSchemaArrayItemElements(schema_root, *items.value, payload,
                                      instance_path, schema_path, result);
  return true;
}

}  // namespace objc3::io::json
