#include "io/json/json_schema_enum_contract_validation.h"

#include <cstddef>

#include "io/json/json_equivalence.h"
#include "io/json/json_schema_errors.h"

namespace objc3::io::json {

void ValidateJsonSchemaEnumContract(const JsonValue &enum_values,
                                    const std::string &schema_path,
                                    JsonSchemaResult &result) {
  if (!enum_values.IsArray()) {
    AddJsonSchemaContractError(result, "invalid_enum", schema_path,
                               "enum must be an array");
    return;
  }
  for (std::size_t i = 0; i < enum_values.AsArray().size(); ++i) {
    for (std::size_t j = i + 1; j < enum_values.AsArray().size(); ++j) {
      if (JsonEquals(enum_values.AsArray()[i], enum_values.AsArray()[j])) {
        AddJsonSchemaContractError(
            result, "duplicate_enum",
            JsonInstanceArrayElementPath(schema_path, j),
            "enum values must be unique");
      }
    }
  }
}

}  // namespace objc3::io::json
