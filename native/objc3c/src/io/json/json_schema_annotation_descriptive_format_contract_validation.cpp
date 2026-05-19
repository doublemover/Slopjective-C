#include "io/json/json_schema_annotation_descriptive_format_contract_validation.h"

#include "io/json/json_schema_errors.h"

namespace objc3::io::json {

void ValidateJsonSchemaFormatAnnotationContract(
    const JsonValue &schema, const std::string &schema_path,
    JsonSchemaResult &result) {
  const JsonValue *value = schema.Find("format");
  if (value != nullptr && !value->IsString()) {
    AddJsonSchemaContractError(
        result, "invalid_annotation",
        JsonSchemaKeywordPath(schema_path, "format"),
        "format must be a string when present");
  }
}

}  // namespace objc3::io::json
