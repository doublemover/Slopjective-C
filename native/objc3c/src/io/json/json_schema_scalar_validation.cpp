#include "io/json/json_schema_scalar_validation.h"

#include "io/json/json_schema_scalar_numeric_dispatch_validation.h"
#include "io/json/json_schema_scalar_string_dispatch_validation.h"

namespace objc3::io::json {

void ValidateJsonSchemaScalarFields(const JsonValue &schema,
                                    const JsonValue &payload,
                                    const std::string &instance_path,
                                    const std::string &schema_path,
                                    JsonSchemaResult &result) {
  ValidateJsonSchemaScalarNumericConstraints(schema, payload, instance_path,
                                             schema_path, result);
  ValidateJsonSchemaScalarStringConstraints(schema, payload, instance_path,
                                            schema_path, result);
}

}  // namespace objc3::io::json
