#include "io/json/json_schema_array_uniqueness_validation.h"

#include "io/json/json_schema_array_uniqueness_duplicate_scan_validation.h"
#include "io/json/json_schema_array_uniqueness_keyword_validation.h"

namespace objc3::io::json {

bool ValidateJsonSchemaArrayUniqueness(const JsonValue &schema,
                                       const JsonValue &payload,
                                       const std::string &instance_path,
                                       const std::string &schema_path,
                                       JsonSchemaResult &result) {
  const JsonSchemaArrayUniquenessKeywordValidation unique_items =
      ValidateJsonSchemaArrayUniqueItemsKeyword(schema, payload, schema_path,
                                                result);
  if (!unique_items.valid) {
    return false;
  }
  if (unique_items.scan_duplicates) {
    ValidateJsonSchemaArrayUniquenessDuplicateScan(
        payload, instance_path, schema_path, result);
  }
  return true;
}

}  // namespace objc3::io::json
