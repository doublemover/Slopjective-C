#include "io/json/json_schema_unsupported_applicator_keyword_contracts.h"

namespace objc3::io::json {

bool IsJsonSchemaApplicatorKeyword(std::string_view key) {
  return key == "$ref" || key == "$defs" || key == "definitions" ||
         key == "allOf" || key == "anyOf" || key == "properties" ||
         key == "additionalProperties" || key == "items" ||
         key == "contains";
}

}  // namespace objc3::io::json
