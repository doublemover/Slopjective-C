#include "io/json/json_schema_annotation_descriptive_title_contract_validation.h"

#include "io/json/json_schema_errors.h"

namespace objc3::io::json {

void ValidateJsonSchemaTitleAnnotationContract(
    const JsonValue &schema, const std::string &schema_path,
    JsonSchemaResult &result) {
  const JsonValue *value = schema.Find("title");
  if (value != nullptr && !value->IsString()) {
    AddJsonSchemaContractError(
        result, "invalid_annotation",
        JsonSchemaKeywordPath(schema_path, "title"),
        "title must be a string when present");
  }
}

}  // namespace objc3::io::json
