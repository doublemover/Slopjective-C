#include "io/json/json_schema_numeric_nonnegative_array_item_count_contract_validation.h"

#include "io/json/json_schema_numeric_nonnegative_array_item_count_max_items_contract_validation.h"
#include "io/json/json_schema_numeric_nonnegative_array_item_count_min_items_contract_validation.h"

namespace objc3::io::json {

void ValidateJsonSchemaNonnegativeArrayItemCountKeywordContracts(
    const JsonValue &schema, const std::string &schema_path,
    JsonSchemaResult &result) {
  ValidateJsonSchemaNonnegativeMinItemsContract(schema, schema_path, result);
  ValidateJsonSchemaNonnegativeMaxItemsContract(schema, schema_path, result);
}

}  // namespace objc3::io::json
