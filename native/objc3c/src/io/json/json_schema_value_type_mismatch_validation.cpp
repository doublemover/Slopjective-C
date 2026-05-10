#include "io/json/json_schema_value_type_mismatch_validation.h"

#include <sstream>

#include "io/json/json_schema_errors.h"
#include "io/json/json_schema_type.h"

namespace objc3::io::json {

void AddJsonSchemaValueTypeMismatchError(
    const JsonValue &schema_type,
    const JsonValue &payload,
    const std::string &instance_path,
    const std::string &schema_path,
    JsonSchemaResult &result) {
  std::ostringstream out;
  out << "expected " << DescribeExpectedJsonSchemaType(schema_type)
      << " but found " << JsonSchemaValueTypeName(payload);
  AddJsonSchemaPayloadError(
      result, "type_mismatch", instance_path,
      JsonSchemaKeywordPath(schema_path, "type"), out.str());
}

}  // namespace objc3::io::json
