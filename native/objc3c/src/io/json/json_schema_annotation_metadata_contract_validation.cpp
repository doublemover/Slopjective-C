#include "io/json/json_schema_annotation_metadata_contract_validation.h"

#include "io/json/json_schema_errors.h"

namespace objc3::io::json {
namespace {

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

void ValidateJsonSchemaIdAnnotationContract(const JsonValue &schema,
                                            const std::string &schema_path,
                                            JsonSchemaResult &result) {
  const JsonValue *value = schema.Find("$id");
  if (value != nullptr && !value->IsString()) {
    AddJsonSchemaContractError(
        result, "invalid_annotation",
        JsonSchemaKeywordPath(schema_path, "$id"),
        "$id must be a string when present");
  }
}

void ValidateJsonSchemaCommentAnnotationContract(
    const JsonValue &schema, const std::string &schema_path,
    JsonSchemaResult &result) {
  const JsonValue *value = schema.Find("$comment");
  if (value != nullptr && !value->IsString()) {
    AddJsonSchemaContractError(
        result, "invalid_annotation",
        JsonSchemaKeywordPath(schema_path, "$comment"),
        "$comment must be a string when present");
  }
}

}  // namespace

void ValidateJsonSchemaMetadataAnnotationContracts(
    const JsonValue &schema, const std::string &schema_path,
    JsonSchemaResult &result) {
  ValidateJsonSchemaSchemaAnnotationContract(schema, schema_path, result);
  ValidateJsonSchemaIdAnnotationContract(schema, schema_path, result);
  ValidateJsonSchemaCommentAnnotationContract(schema, schema_path, result);
}

}  // namespace objc3::io::json
