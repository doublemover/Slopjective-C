#include "io/json/json_schema_type_contract_validation.h"

#include <cstddef>
#include <string_view>

#include "io/json/json_schema_errors.h"

namespace objc3::io::json {
namespace {

bool IsSupportedJsonSchemaTypeName(std::string_view type) {
  return type == "null" || type == "boolean" || type == "number" ||
         type == "integer" || type == "string" || type == "array" ||
         type == "object";
}

}  // namespace

void ValidateJsonSchemaTypeContract(const JsonValue &schema_type,
                                    const std::string &schema_path,
                                    JsonSchemaResult &result) {
  if (schema_type.IsString()) {
    if (!IsSupportedJsonSchemaTypeName(schema_type.AsString())) {
      AddJsonSchemaContractError(result, "unsupported_type", schema_path,
                                 "unsupported JSON Schema type " +
                                     schema_type.AsString());
    }
    return;
  }
  if (!schema_type.IsArray()) {
    AddJsonSchemaContractError(result, "invalid_type", schema_path,
                               "type must be a string or an array of strings");
    return;
  }
  if (schema_type.AsArray().empty()) {
    AddJsonSchemaContractError(result, "invalid_type", schema_path,
                               "type array must not be empty");
    return;
  }
  for (std::size_t i = 0; i < schema_type.AsArray().size(); ++i) {
    const JsonValue &entry = schema_type.AsArray()[i];
    const std::string entry_path = JsonInstanceArrayElementPath(schema_path, i);
    if (!entry.IsString()) {
      AddJsonSchemaContractError(result, "invalid_type_entry", entry_path,
                                 "type array entries must be strings");
      continue;
    }
    if (!IsSupportedJsonSchemaTypeName(entry.AsString())) {
      AddJsonSchemaContractError(result, "unsupported_type", entry_path,
                                 "unsupported JSON Schema type " +
                                     entry.AsString());
    }
  }
}

}  // namespace objc3::io::json
