#include "io/json/json_schema_value_type_keyword_validation.h"

namespace objc3::io::json {

const JsonValue *FindJsonSchemaValueTypeKeyword(const JsonValue &schema) {
  return schema.Find("type");
}

}  // namespace objc3::io::json
