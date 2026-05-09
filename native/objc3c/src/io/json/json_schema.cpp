#include "io/json/json_schema.h"

#include "io/json/json_schema_annotation_contract_validation.h"
#include "io/json/json_schema_applicator_contract_validation.h"
#include "io/json/json_schema_composition_contract_validation.h"
#include "io/json/json_schema_contract_validation.h"
#include "io/json/json_schema_enum_contract_validation.h"
#include "io/json/json_schema_errors.h"
#include "io/json/json_schema_numeric_contract_validation.h"
#include "io/json/json_schema_pattern_contract_validation.h"
#include "io/json/json_schema_ref_contract_validation.h"
#include "io/json/json_schema_required_contract_validation.h"
#include "io/json/json_schema_type_contract_validation.h"
#include "io/json/json_schema_unique_items_contract_validation.h"
#include "io/json/json_schema_unsupported_keyword_contracts.h"
#include "io/json/json_schema_validation.h"

namespace objc3::io::json {

void ValidateJsonSchemaNodeContract(const JsonValue &schema_root,
                                    const JsonValue &schema,
                                    const std::string &schema_path,
                                    JsonSchemaResult &result) {
  if (!schema.IsObject()) {
    AddJsonSchemaContractError(result, "invalid_schema_node", schema_path,
                               "schema node must be a JSON object");
    return;
  }
  ValidateJsonSchemaUnsupportedKeywordContracts(schema, schema_path, result);
  ValidateJsonSchemaAnnotationContracts(schema, schema_path, result);
  ValidateJsonSchemaRefContract(schema_root, schema, schema_path, result);
  if (const JsonValue *type = schema.Find("type"); type != nullptr) {
    ValidateJsonSchemaTypeContract(*type,
                                   JsonSchemaKeywordPath(schema_path, "type"),
                                   result);
  }
  ValidateJsonSchemaCompositionContracts(schema_root, schema, schema_path,
                                         result);
  ValidateJsonSchemaApplicatorContracts(schema_root, schema, schema_path,
                                        result);
  if (const JsonValue *required = schema.Find("required");
      required != nullptr) {
    ValidateJsonSchemaRequiredContract(
        *required, JsonSchemaKeywordPath(schema_path, "required"), result);
  }
  if (const JsonValue *enum_values = schema.Find("enum");
      enum_values != nullptr) {
    ValidateJsonSchemaEnumContract(
        *enum_values, JsonSchemaKeywordPath(schema_path, "enum"), result);
  }
  ValidateJsonSchemaNumericAssertionContracts(schema, schema_path, result);
  ValidateJsonSchemaUniqueItemsContract(schema, schema_path, result);
  ValidateJsonSchemaPatternContract(schema, schema_path, result);
}

JsonSchemaResult ValidateJsonSchema(const JsonValue &schema,
                                    const JsonValue &payload) {
  JsonSchemaResult result;
  ValidateJsonSchemaNodeContract(schema, schema, "$", result);
  if (!result.ok) {
    return result;
  }
  ValidateJsonSchemaNode(schema, schema, payload, "$", "$", result);
  return result;
}

}  // namespace objc3::io::json
