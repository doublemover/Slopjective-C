#include "io/json/json_schema_object_required_validation.h"

#include "io/json/json_schema_object_required_entry_validation.h"
#include "io/json/json_schema_object_required_keyword_validation.h"

namespace objc3::io::json {

void ValidateJsonSchemaRequiredProperties(const JsonValue &schema,
                                          const JsonValue &payload,
                                          const std::string &instance_path,
                                          const std::string &schema_path,
                                          JsonSchemaResult &result) {
  const JsonValue *required = ValidateJsonSchemaObjectRequiredKeyword(
      schema, payload, schema_path, result);
  if (required == nullptr) {
    return;
  }
  ValidateJsonSchemaObjectRequiredEntries(*required, payload, instance_path,
                                          schema_path, result);
}

}  // namespace objc3::io::json
