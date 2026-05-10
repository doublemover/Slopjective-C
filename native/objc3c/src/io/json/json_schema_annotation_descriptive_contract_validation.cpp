#include "io/json/json_schema_annotation_descriptive_contract_validation.h"

#include "io/json/json_schema_annotation_descriptive_description_contract_validation.h"
#include "io/json/json_schema_annotation_descriptive_format_contract_validation.h"
#include "io/json/json_schema_annotation_descriptive_title_contract_validation.h"

namespace objc3::io::json {

void ValidateJsonSchemaDescriptiveAnnotationContracts(
    const JsonValue &schema, const std::string &schema_path,
    JsonSchemaResult &result) {
  ValidateJsonSchemaTitleAnnotationContract(schema, schema_path, result);
  ValidateJsonSchemaDescriptionAnnotationContract(schema, schema_path, result);
  ValidateJsonSchemaFormatAnnotationContract(schema, schema_path, result);
}

}  // namespace objc3::io::json
