#include "io/json/json_schema_unsupported_keyword_contracts.h"

#include <string_view>

#include "io/json/json_schema_unsupported_annotation_keyword_contracts.h"
#include "io/json/json_schema_unsupported_applicator_keyword_contracts.h"
#include "io/json/json_schema_unsupported_assertion_keyword_contracts.h"
#include "io/json/json_schema_errors.h"

namespace objc3::io::json {
namespace {

bool IsSupportedJsonSchemaKeyword(std::string_view key) {
  return IsJsonSchemaAnnotationKeyword(key) ||
         IsJsonSchemaApplicatorKeyword(key) ||
         IsJsonSchemaAssertionKeyword(key);
}

}  // namespace

void ValidateJsonSchemaUnsupportedKeywordContracts(
    const JsonValue &schema, const std::string &schema_path,
    JsonSchemaResult &result) {
  for (const auto &[key, value] : schema.AsObject()) {
    (void)value;
    if (!IsSupportedJsonSchemaKeyword(key)) {
      AddJsonSchemaContractError(
          result, "unsupported_keyword", JsonSchemaKeywordPath(schema_path, key),
          "unsupported JSON Schema keyword " + key);
    }
  }
}

}  // namespace objc3::io::json
