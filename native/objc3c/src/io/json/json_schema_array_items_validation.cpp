#include "io/json/json_schema_array_items_validation.h"

#include <cstddef>

#include "io/json/json_schema_errors.h"
#include "io/json/json_schema_validation.h"

namespace objc3::io::json {

bool ValidateJsonSchemaArrayItems(const JsonValue &schema_root,
                                  const JsonValue &schema,
                                  const JsonValue &payload,
                                  const std::string &instance_path,
                                  const std::string &schema_path,
                                  JsonSchemaResult &result) {
  const JsonValue *items = schema.Find("items");
  if (items != nullptr && payload.IsArray()) {
    if (!items->IsObject()) {
      AddJsonSchemaContractError(result, "invalid_items",
                                 JsonSchemaKeywordPath(schema_path, "items"),
                                 "items must be a schema object");
      return false;
    }
    const auto &array = payload.AsArray();
    for (std::size_t i = 0; i < array.size(); ++i) {
      ValidateJsonSchemaNode(schema_root, *items, array[i],
                             JsonInstanceArrayElementPath(instance_path, i),
                             JsonSchemaKeywordPath(schema_path, "items"),
                             result);
    }
  }
  return true;
}

}  // namespace objc3::io::json
