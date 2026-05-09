#include "io/json/json_schema_value_type_validation.h"

#include <sstream>

#include "io/json/json_schema_errors.h"
#include "io/json/json_schema_type.h"

namespace objc3::io::json {

bool ValidateJsonSchemaValueTypeKeyword(const JsonValue &schema,
                                        const JsonValue &payload,
                                        const std::string &instance_path,
                                        const std::string &schema_path,
                                        JsonSchemaResult &result) {
  if (const JsonValue *schema_type = schema.Find("type");
      schema_type != nullptr) {
    if (!JsonSchemaMatchesType(*schema_type, payload)) {
      std::ostringstream out;
      out << "expected " << DescribeExpectedJsonSchemaType(*schema_type)
          << " but found " << JsonSchemaValueTypeName(payload);
      AddJsonSchemaPayloadError(
          result, "type_mismatch", instance_path,
          JsonSchemaKeywordPath(schema_path, "type"), out.str());
      return false;
    }
  }

  return true;
}

}  // namespace objc3::io::json
