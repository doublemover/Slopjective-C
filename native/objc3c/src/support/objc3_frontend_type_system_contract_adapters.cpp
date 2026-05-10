#include "artifacts/objc3_frontend_type_system_contract_artifacts.h"

#include <string>

#include "lower/contracts/optional_keypath_lowering_contracts.h"
#include "sema/objc3_sema_contract_core.h"
#include "sema/objc3_sema_contract_type_handoff.h"
#include "sema/objc3_sema_pass_manager_contract_flow.h"

namespace objc3::artifacts::frontend {
namespace {

#include "objc3_frontend_type_system_contract_adapter_records.inc"

}  // namespace

#include "objc3_frontend_type_system_contract_adapter_semantic_model.inc"
#include "objc3_frontend_type_system_contract_adapter_parity_surface.inc"
#include "objc3_frontend_type_system_contract_adapter_json.inc"

}  // namespace objc3::artifacts::frontend
