#include "io/json/json_schema_type_name_contract_validation.h"

namespace objc3::io::json {

bool IsSupportedJsonSchemaTypeName(std::string_view type) {
  return type == "null" || type == "boolean" || type == "number" ||
         type == "integer" || type == "string" || type == "array" ||
         type == "object";
}

}  // namespace objc3::io::json
