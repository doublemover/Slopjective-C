#include "io/json/json_schema_array_uniqueness_duplicates_validation.h"

#include <cstddef>

#include "io/json/json_equivalence.h"
#include "io/json/json_schema_errors.h"

namespace objc3::io::json {

void ValidateJsonSchemaArrayUniqueItemDuplicates(
    const JsonValue::Array &array, const std::string &instance_path,
    const std::string &unique_items_schema_path, JsonSchemaResult &result) {
  bool duplicate = false;
  for (std::size_t i = 0; i < array.size(); ++i) {
    for (std::size_t j = i + 1; j < array.size(); ++j) {
      if (JsonEquals(array[i], array[j])) {
        duplicate = true;
        break;
      }
    }
    if (duplicate) {
      AddJsonSchemaPayloadError(result, "unique_items", instance_path,
                                unique_items_schema_path,
                                "array contains duplicate items");
      break;
    }
  }
}

}  // namespace objc3::io::json
