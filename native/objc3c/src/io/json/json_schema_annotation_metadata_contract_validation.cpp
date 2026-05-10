#include "io/json/json_schema_annotation_metadata_contract_validation.h"

#include "io/json/json_schema_annotation_comment_contract_validation.h"
#include "io/json/json_schema_annotation_id_contract_validation.h"
#include "io/json/json_schema_annotation_schema_contract_validation.h"

namespace objc3::io::json {

void ValidateJsonSchemaMetadataAnnotationContracts(
    const JsonValue &schema, const std::string &schema_path,
    JsonSchemaResult &result) {
  ValidateJsonSchemaSchemaAnnotationContract(schema, schema_path, result);
  ValidateJsonSchemaIdAnnotationContract(schema, schema_path, result);
  ValidateJsonSchemaCommentAnnotationContract(schema, schema_path, result);
}

}  // namespace objc3::io::json
