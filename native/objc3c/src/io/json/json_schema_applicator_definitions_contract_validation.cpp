#include "io/json/json_schema_applicator_definitions_contract_validation.h"

#include "io/json/json_schema_contract_validation.h"
#include "io/json/json_schema_errors.h"

namespace objc3::io::json {

void ValidateJsonSchemaDefinitionsContract(const JsonValue &schema_root,
                                           const JsonValue &schema,
                                           const std::string &schema_path,
                                           JsonSchemaResult &result) {
  const JsonValue *definitions = schema.Find("definitions");
  if (definitions == nullptr) {
    return;
  }
  const std::string definitions_path =
      JsonSchemaKeywordPath(schema_path, "definitions");
  if (!definitions->IsObject()) {
    AddJsonSchemaContractError(result, "invalid_schema_map", definitions_path,
                               "schema map must be an object");
    return;
  }
  for (const auto &[key, child] : definitions->AsObject()) {
    ValidateJsonSchemaNodeContract(
        schema_root, child, JsonInstancePropertyPath(definitions_path, key),
        result);
  }
}

}  // namespace objc3::io::json
