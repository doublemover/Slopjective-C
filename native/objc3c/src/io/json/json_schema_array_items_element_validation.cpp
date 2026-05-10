#include "io/json/json_schema_array_items_element_validation.h"

#include <cstddef>

#include "io/json/json_schema_errors.h"
#include "io/json/json_schema_validation.h"

namespace objc3::io::json {

void ValidateJsonSchemaArrayItemElements(
    const JsonValue &schema_root,
    const JsonValue &items,
    const JsonValue &payload,
    const std::string &instance_path,
    const std::string &schema_path,
    JsonSchemaResult &result) {
  const auto &array = payload.AsArray();
  for (std::size_t i = 0; i < array.size(); ++i) {
    ValidateJsonSchemaNode(schema_root, items, array[i],
                           JsonInstanceArrayElementPath(instance_path, i),
                           JsonSchemaKeywordPath(schema_path, "items"),
                           result);
  }
}

}  // namespace objc3::io::json
