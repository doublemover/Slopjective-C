#include "artifacts/objc3_frontend_artifact_dispatch_accessor_abi_manifest.h"

#include <ostream>

#include "lower/contracts/dispatch_surface_classification_contracts.h"
#include "lower/contracts/message_send_selector_lowering_contracts.h"
#include "lower/contracts/object_model_lowering_contracts.h"
#include "lower/contracts/ownership_runtime_accessor_helper_contracts.h"
#include "lower/contracts/ownership_runtime_memory_management_contracts.h"
#include "lower/contracts/runtime_dispatch_abi_contracts.h"

namespace objc3::artifacts::frontend {

void WriteDispatchAccessorRuntimeAbiSurface(
    std::ostream &manifest,
    const Objc3RuntimeLinkHostLinkContract &runtime_link_host_link_contract,
    const Objc3PropertySynthesisIvarBindingContract
        &property_synthesis_ivar_binding_contract,
    const Objc3DispatchSurfaceClassificationContract
        &dispatch_surface_classification_contract,
    const Objc3MessageSendSelectorLoweringContract
        &message_send_selector_lowering_contract) {
  manifest << "  \"dispatch_accessor_runtime_abi_surface\":{\"contract_id\":"
           << "\"objc3c.runtime.dispatch_accessor.abi.surface.v1\""
           << ",\"abi_boundary_model\":"
           << "\"public-dispatch-entrypoint-plus-private-testing-snapshot-and-property-helper-surface\""
           << ",\"public_header_path\":\"native/objc3c/src/runtime/public/objc3_runtime_api.h\""
           << ",\"private_header_path\":\"native/objc3c/src/runtime/objc3_runtime_bootstrap_internal.h\""
           << ",\"runtime_dispatch_symbol\":\""
           << runtime_link_host_link_contract.runtime_dispatch_symbol
           << "\",\"dispatch_state_snapshot_symbol\":\"objc3_runtime_copy_dispatch_state_for_testing\""
           << ",\"method_cache_state_snapshot_symbol\":\"objc3_runtime_copy_method_cache_state_for_testing\""
           << ",\"method_cache_entry_snapshot_symbol\":\"objc3_runtime_copy_method_cache_entry_for_testing\""
           << ",\"property_registry_state_snapshot_symbol\":\"objc3_runtime_copy_property_registry_state_for_testing\""
           << ",\"property_entry_snapshot_symbol\":\"objc3_runtime_copy_property_entry_for_testing\""
           << ",\"arc_debug_state_snapshot_symbol\":\"objc3_runtime_copy_arc_debug_state_for_testing\""
           << ",\"current_property_read_symbol\":\""
           << kObjc3RuntimeReadCurrentPropertyI32Symbol
           << "\",\"current_property_write_symbol\":\""
           << kObjc3RuntimeWriteCurrentPropertyI32Symbol
           << "\",\"current_property_exchange_symbol\":\""
           << kObjc3RuntimeExchangeCurrentPropertyI32Symbol
           << "\",\"bind_current_property_context_symbol\":\"objc3_runtime_bind_current_property_context_for_testing\""
           << ",\"clear_current_property_context_symbol\":\"objc3_runtime_clear_current_property_context_for_testing\""
           << ",\"weak_current_property_load_symbol\":\""
           << kObjc3RuntimeLoadWeakCurrentPropertyI32Symbol
           << "\",\"weak_current_property_store_symbol\":\""
           << kObjc3RuntimeStoreWeakCurrentPropertyI32Symbol
           << "\",\"retain_symbol\":\"" << kObjc3RuntimeRetainI32Symbol
           << "\",\"release_symbol\":\"" << kObjc3RuntimeReleaseI32Symbol
           << "\",\"autorelease_symbol\":\""
           << kObjc3RuntimeAutoreleaseI32Symbol
           << "\",\"private_testing_surface_only\":true"
           << ",\"deterministic\":"
           << ((property_synthesis_ivar_binding_contract.deterministic &&
                dispatch_surface_classification_contract.deterministic &&
                message_send_selector_lowering_contract.deterministic &&
                runtime_link_host_link_contract.deterministic)
                   ? "true"
                   : "false")
           << "},\n";
}

}  // namespace objc3::artifacts::frontend
