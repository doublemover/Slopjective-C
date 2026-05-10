#include "io/json/json_schema_numeric_nonnegative_contract_validation.h"

#include "io/json/json_schema_numeric_nonnegative_array_item_count_contract_validation.h"
#include "io/json/json_schema_numeric_nonnegative_string_length_contract_validation.h"

namespace objc3::io::json {

void ValidateJsonSchemaNonnegativeNumericKeywordContracts(
    const JsonValue &schema, const std::string &schema_path,
    JsonSchemaResult &result) {
  ValidateJsonSchemaNonnegativeStringLengthKeywordContracts(schema,
                                                            schema_path,
                                                            result);
  ValidateJsonSchemaNonnegativeArrayItemCountKeywordContracts(schema,
                                                              schema_path,
                                                              result);
}

}  // namespace objc3::io::json
