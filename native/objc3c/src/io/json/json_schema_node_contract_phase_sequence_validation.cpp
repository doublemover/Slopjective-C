#include "io/json/json_schema_node_contract_phase_sequence_validation.h"

#include "io/json/json_schema_annotation_contract_validation.h"
#include "io/json/json_schema_applicator_contract_validation.h"
#include "io/json/json_schema_assertion_contract_validation.h"
#include "io/json/json_schema_composition_contract_validation.h"
#include "io/json/json_schema_node_contract_guard_validation.h"
#include "io/json/json_schema_ref_contract_validation.h"
#include "io/json/json_schema_unsupported_keyword_contracts.h"

namespace objc3::io::json {

void ValidateJsonSchemaNodeContractPhaseSequence(
    const JsonValue &schema_root,
    const JsonValue &schema,
    const std::string &schema_path,
    JsonSchemaResult &result) {
  if (!ValidateJsonSchemaNodeContractGuard(schema, schema_path, result)) {
    return;
  }
  ValidateJsonSchemaUnsupportedKeywordContracts(schema, schema_path, result);
  ValidateJsonSchemaAnnotationContracts(schema, schema_path, result);
  ValidateJsonSchemaRefContract(schema_root, schema, schema_path, result);
  ValidateJsonSchemaPreRecursiveAssertionContracts(schema, schema_path, result);
  ValidateJsonSchemaCompositionContracts(schema_root, schema, schema_path,
                                         result);
  ValidateJsonSchemaApplicatorContracts(schema_root, schema, schema_path,
                                        result);
  ValidateJsonSchemaPostRecursiveAssertionContracts(schema, schema_path,
                                                    result);
}

}  // namespace objc3::io::json
