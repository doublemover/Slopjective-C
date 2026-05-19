#include "io/json/json_schema_numeric_nonnegative_string_length_contract_validation.h"

#include "io/json/json_schema_numeric_nonnegative_string_length_max_length_contract_validation.h"
#include "io/json/json_schema_numeric_nonnegative_string_length_min_length_contract_validation.h"

namespace objc3::io::json {

void ValidateJsonSchemaNonnegativeStringLengthKeywordContracts(
    const JsonValue &schema, const std::string &schema_path,
    JsonSchemaResult &result) {
  ValidateJsonSchemaNonnegativeMinLengthContract(schema, schema_path, result);
  ValidateJsonSchemaNonnegativeMaxLengthContract(schema, schema_path, result);
}

}  // namespace objc3::io::json
