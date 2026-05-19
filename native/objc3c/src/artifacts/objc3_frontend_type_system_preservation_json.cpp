#include "artifacts/objc3_frontend_type_system_contract_artifacts.h"

#include <algorithm>
#include <cstddef>
#include <sstream>
#include <string>
#include <vector>

#include "ast/objc3_ast_contracts.h"
#include "io/objc3_json.h"
#include "runtime/metadata/runtime_metadata_model.h"

namespace objc3::artifacts::frontend {
namespace {

#include "artifacts/objc3_frontend_type_system_preservation_json_common.inc"
#include "artifacts/objc3_frontend_type_system_preservation_json_generic_inventory.inc"
#include "artifacts/objc3_frontend_type_system_preservation_json_protocol_inventory.inc"

}  // namespace

#include "artifacts/objc3_frontend_type_system_preservation_json_generic_emit.inc"
#include "artifacts/objc3_frontend_type_system_preservation_json_protocol_emit.inc"

}  // namespace objc3::artifacts::frontend
