#include "io/json/json_schema_contract_validation.h"

#include "io/json/json_schema_node_contract_phase_sequence_validation.h"

namespace objc3::io::json {

void ValidateJsonSchemaNodeContract(const JsonValue &schema_root,
                                    const JsonValue &schema,
                                    const std::string &schema_path,
                                    JsonSchemaResult &result) {
  ValidateJsonSchemaNodeContractPhaseSequence(schema_root, schema, schema_path,
                                              result);
}

}  // namespace objc3::io::json
