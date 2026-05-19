#include "io/json/json_schema_applicator_defs_contract_validation.h"

#include "io/json/json_schema_contract_validation.h"
#include "io/json/json_schema_errors.h"

namespace objc3::io::json {

void ValidateJsonSchemaDefsContract(const JsonValue &schema_root,
                                    const JsonValue &schema,
                                    const std::string &schema_path,
                                    JsonSchemaResult &result) {
  const JsonValue *defs = schema.Find("$defs");
  if (defs == nullptr) {
    return;
  }
  const std::string defs_path = JsonSchemaKeywordPath(schema_path, "$defs");
  if (!defs->IsObject()) {
    AddJsonSchemaContractError(result, "invalid_schema_map", defs_path,
                               "schema map must be an object");
    return;
  }
  for (const auto &[key, child] : defs->AsObject()) {
    ValidateJsonSchemaNodeContract(
        schema_root, child, JsonInstancePropertyPath(defs_path, key), result);
  }
}

}  // namespace objc3::io::json
