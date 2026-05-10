#include "io/json/json_schema_array_uniqueness_duplicate_scan_validation.h"

#include "io/json/json_schema_array_uniqueness_duplicates_validation.h"
#include "io/json/json_schema_errors.h"

namespace objc3::io::json {

void ValidateJsonSchemaArrayUniquenessDuplicateScan(
    const JsonValue &payload,
    const std::string &instance_path,
    const std::string &schema_path,
    JsonSchemaResult &result) {
  ValidateJsonSchemaArrayUniqueItemDuplicates(
      payload.AsArray(), instance_path,
      JsonSchemaKeywordPath(schema_path, "uniqueItems"), result);
}

}  // namespace objc3::io::json
