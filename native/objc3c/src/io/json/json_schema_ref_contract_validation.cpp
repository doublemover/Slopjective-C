#include "io/json/json_schema_ref_contract_validation.h"

#include "io/json/json_pointer.h"
#include "io/json/json_schema_errors.h"

namespace objc3::io::json {

void ValidateJsonSchemaRefContract(const JsonValue &schema_root,
                                   const JsonValue &schema,
                                   const std::string &schema_path,
                                   JsonSchemaResult &result) {
  if (const JsonValue *ref = schema.Find("$ref"); ref != nullptr) {
    if (!ref->IsString()) {
      AddJsonSchemaContractError(result, "invalid_ref",
                                 JsonSchemaKeywordPath(schema_path, "$ref"),
                                 "$ref must be a local JSON pointer string");
    } else if (ResolveLocalJsonPointerRef(schema_root, ref->AsString()) ==
               nullptr) {
      AddJsonSchemaContractError(
          result, "unresolved_ref", JsonSchemaKeywordPath(schema_path, "$ref"),
          "unresolved schema reference " + ref->AsString());
    }
  }
}

}  // namespace objc3::io::json
