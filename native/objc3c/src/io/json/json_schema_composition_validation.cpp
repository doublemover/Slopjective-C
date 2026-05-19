#include "io/json/json_schema_composition_validation.h"

#include "io/json/json_schema_composition_all_of_dispatch_validation.h"
#include "io/json/json_schema_composition_any_of_dispatch_validation.h"

namespace objc3::io::json {

void ValidateJsonSchemaCompositionKeywords(const JsonValue &schema_root,
                                           const JsonValue &schema,
                                           const JsonValue &payload,
                                           const std::string &instance_path,
                                           const std::string &schema_path,
                                           JsonSchemaResult &result) {
  ValidateJsonSchemaAllOfCompositionDispatch(schema_root, schema, payload,
                                             instance_path, schema_path,
                                             result);
  if (!ValidateJsonSchemaAnyOfCompositionDispatch(schema_root, schema, payload,
                                                  instance_path, schema_path,
                                                  result)) {
    return;
  }
}

}  // namespace objc3::io::json
