#include "io/json/json_schema_annotation_descriptive_contract_validation.h"

#include "io/json/json_schema_errors.h"

namespace objc3::io::json {
namespace {

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

void ValidateJsonSchemaDescriptionAnnotationContract(
    const JsonValue &schema, const std::string &schema_path,
    JsonSchemaResult &result) {
  const JsonValue *value = schema.Find("description");
  if (value != nullptr && !value->IsString()) {
    AddJsonSchemaContractError(
        result, "invalid_annotation",
        JsonSchemaKeywordPath(schema_path, "description"),
        "description must be a string when present");
  }
}

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

}  // namespace

void ValidateJsonSchemaDescriptiveAnnotationContracts(
    const JsonValue &schema, const std::string &schema_path,
    JsonSchemaResult &result) {
  ValidateJsonSchemaTitleAnnotationContract(schema, schema_path, result);
  ValidateJsonSchemaDescriptionAnnotationContract(schema, schema_path, result);
  ValidateJsonSchemaFormatAnnotationContract(schema, schema_path, result);
}

}  // namespace objc3::io::json
