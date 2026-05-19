#include "io/json/json_schema_unsupported_assertion_keyword_contracts.h"

namespace objc3::io::json {

bool IsJsonSchemaAssertionKeyword(std::string_view key) {
  return key == "type" || key == "required" || key == "const" ||
         key == "enum" || key == "minimum" || key == "maximum" ||
         key == "minLength" || key == "maxLength" || key == "pattern" ||
         key == "minItems" || key == "maxItems" || key == "uniqueItems";
}

}  // namespace objc3::io::json
