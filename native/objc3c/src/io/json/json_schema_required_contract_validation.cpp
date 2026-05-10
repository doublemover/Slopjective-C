#include "io/json/json_schema_required_contract_validation.h"

#include "io/json/json_schema_required_contract_array_shape_validation.h"
#include "io/json/json_schema_required_contract_entry_iteration_validation.h"

namespace objc3::io::json {

void ValidateJsonSchemaRequiredContract(const JsonValue &required,
                                        const std::string &schema_path,
                                        JsonSchemaResult &result) {
  if (!ValidateJsonSchemaRequiredArrayShape(required, schema_path, result)) {
    return;
  }
  ValidateJsonSchemaRequiredEntryIteration(required.AsArray(), schema_path,
                                           result);
}

}  // namespace objc3::io::json
