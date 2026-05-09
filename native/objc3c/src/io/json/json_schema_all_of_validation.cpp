#include "io/json/json_schema_all_of_validation.h"

#include <cstddef>

#include "io/json/json_schema_errors.h"
#include "io/json/json_schema_validation.h"

namespace objc3::io::json {

void ValidateJsonSchemaAllOf(const JsonValue &schema_root,
                             const JsonValue &schema,
                             const JsonValue &payload,
                             const std::string &instance_path,
                             const std::string &schema_path,
                             JsonSchemaResult &result) {
  const JsonValue *all_of = schema.Find("allOf");
  if (all_of != nullptr) {
    if (!all_of->IsArray()) {
      AddJsonSchemaContractError(
          result, "invalid_all_of", JsonSchemaKeywordPath(schema_path, "allOf"),
          "allOf must be an array of schema objects");
    } else {
      const JsonValue::Array &candidates = all_of->AsArray();
      for (std::size_t i = 0; i < candidates.size(); ++i) {
        ValidateJsonSchemaNode(schema_root, candidates[i], payload,
                               instance_path,
                               JsonSchemaArrayElementPath(schema_path, "allOf",
                                                          i),
                               result);
      }
    }
  }
}

}  // namespace objc3::io::json
