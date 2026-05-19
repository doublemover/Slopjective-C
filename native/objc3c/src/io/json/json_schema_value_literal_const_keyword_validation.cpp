#include "io/json/json_schema_value_literal_const_keyword_validation.h"

namespace objc3::io::json {

const JsonValue *FindJsonSchemaConstLiteralKeyword(const JsonValue &schema) {
  return schema.Find("const");
}

}  // namespace objc3::io::json
