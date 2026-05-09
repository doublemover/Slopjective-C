#include "io/json/json_schema_applicator_properties_contract_validation.h"

#include "io/json/json_schema_contract_validation.h"
#include "io/json/json_schema_errors.h"

namespace objc3::io::json {

void ValidateJsonSchemaPropertiesContract(const JsonValue &schema_root,
                                          const JsonValue &schema,
                                          const std::string &schema_path,
                                          JsonSchemaResult &result) {
  const JsonValue *properties = schema.Find("properties");
  if (properties == nullptr) {
    return;
  }
  const std::string properties_path =
      JsonSchemaKeywordPath(schema_path, "properties");
  if (!properties->IsObject()) {
    AddJsonSchemaContractError(result, "invalid_schema_map", properties_path,
                               "schema map must be an object");
    return;
  }
  for (const auto &[key, child] : properties->AsObject()) {
    ValidateJsonSchemaNodeContract(
        schema_root, child, JsonInstancePropertyPath(properties_path, key),
        result);
  }
}

}  // namespace objc3::io::json
