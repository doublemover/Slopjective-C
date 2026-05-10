#include "io/json/json_schema_annotation_schema_contract_validation.h"

#include "io/json/json_schema_errors.h"

namespace objc3::io::json {

void ValidateJsonSchemaSchemaAnnotationContract(
    const JsonValue &schema, const std::string &schema_path,
    JsonSchemaResult &result) {
  const JsonValue *value = schema.Find("$schema");
  if (value != nullptr && !value->IsString()) {
    AddJsonSchemaContractError(
        result, "invalid_annotation",
        JsonSchemaKeywordPath(schema_path, "$schema"),
        "$schema must be a string when present");
  }
}

}  // namespace objc3::io::json
