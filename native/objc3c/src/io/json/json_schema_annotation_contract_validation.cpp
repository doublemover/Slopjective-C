#include "io/json/json_schema_annotation_contract_validation.h"

#include "io/json/json_schema_annotation_descriptive_contract_validation.h"
#include "io/json/json_schema_annotation_metadata_contract_validation.h"

namespace objc3::io::json {

void ValidateJsonSchemaAnnotationContracts(const JsonValue &schema,
                                           const std::string &schema_path,
                                           JsonSchemaResult &result) {
  ValidateJsonSchemaMetadataAnnotationContracts(schema, schema_path, result);
  ValidateJsonSchemaDescriptiveAnnotationContracts(schema, schema_path, result);
}

}  // namespace objc3::io::json
