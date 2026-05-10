#include "io/json/json_schema_enum_duplicates_contract_validation.h"

#include <cstddef>

#include "io/json/json_equivalence.h"
#include "io/json/json_schema_errors.h"

namespace objc3::io::json {

void ValidateJsonSchemaEnumDuplicateValuesContract(
    const JsonValue::Array &enum_values,
    const std::string &schema_path,
    JsonSchemaResult &result) {
  for (std::size_t i = 0; i < enum_values.size(); ++i) {
    for (std::size_t j = i + 1; j < enum_values.size(); ++j) {
      if (JsonEquals(enum_values[i], enum_values[j])) {
        AddJsonSchemaContractError(
            result, "duplicate_enum",
            JsonInstanceArrayElementPath(schema_path, j),
            "enum values must be unique");
      }
    }
  }
}

}  // namespace objc3::io::json
