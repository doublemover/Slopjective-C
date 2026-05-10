#include "artifacts/objc3_frontend_artifact_dispatch_accessor_abi_manifest.h"

#include <ostream>

#include "artifacts/objc3_frontend_artifact_dispatch_accessor_manifest_contracts.h"
#include "artifacts/objc3_frontend_artifact_storage_accessor_manifest_contracts.h"

namespace objc3::artifacts::frontend {

void WriteDispatchAccessorRuntimeAbiSurface(
    std::ostream &manifest,
    const Objc3DispatchAccessorRuntimeAbiFields
        &dispatch_accessor_runtime_abi_fields) {
  manifest << "  \"dispatch_accessor_runtime_abi_surface\":{\"contract_id\":"
           << "\"" << kDispatchAccessorRuntimeAbiSurfaceContractId << "\""
           << ",\"abi_boundary_model\":"
           << "\"" << kDispatchAccessorRuntimeAbiBoundaryModel << "\""
           << ",\"public_header_path\":\"native/objc3c/src/runtime/public/objc3_runtime_api.h\""
           << ",\"private_header_path\":\"native/objc3c/src/runtime/objc3_runtime_bootstrap_internal.h\""
           << ",\"runtime_dispatch_symbol\":\""
           << dispatch_accessor_runtime_abi_fields.runtime_dispatch_symbol
           << "\",\"dispatch_state_snapshot_symbol\":\"objc3_runtime_copy_dispatch_state_for_testing\""
           << ",\"method_cache_state_snapshot_symbol\":\"objc3_runtime_copy_method_cache_state_for_testing\""
           << ",\"method_cache_entry_snapshot_symbol\":\"objc3_runtime_copy_method_cache_entry_for_testing\""
           << ",\"property_registry_state_snapshot_symbol\":\"objc3_runtime_copy_property_registry_state_for_testing\""
           << ",\"property_entry_snapshot_symbol\":\"objc3_runtime_copy_property_entry_for_testing\""
           << ",\"arc_debug_state_snapshot_symbol\":\"objc3_runtime_copy_arc_debug_state_for_testing\""
           << ",\"current_property_read_symbol\":\""
           << kStorageAccessorReadCurrentPropertyI32Symbol
           << "\",\"current_property_write_symbol\":\""
           << kStorageAccessorWriteCurrentPropertyI32Symbol
           << "\",\"current_property_exchange_symbol\":\""
           << kStorageAccessorExchangeCurrentPropertyI32Symbol
           << "\",\"bind_current_property_context_symbol\":\"objc3_runtime_bind_current_property_context_for_testing\""
           << ",\"clear_current_property_context_symbol\":\"objc3_runtime_clear_current_property_context_for_testing\""
           << ",\"weak_current_property_load_symbol\":\""
           << kStorageAccessorLoadWeakCurrentPropertyI32Symbol
           << "\",\"weak_current_property_store_symbol\":\""
           << kStorageAccessorStoreWeakCurrentPropertyI32Symbol
           << "\",\"retain_symbol\":\"" << kDispatchAccessorRuntimeRetainI32Symbol
           << "\",\"release_symbol\":\"" << kDispatchAccessorRuntimeReleaseI32Symbol
           << "\",\"autorelease_symbol\":\""
           << kDispatchAccessorRuntimeAutoreleaseI32Symbol
           << "\",\"private_testing_surface_only\":true"
           << ",\"deterministic\":"
           << (dispatch_accessor_runtime_abi_fields.deterministic ? "true"
                                                                  : "false")
           << "},\n";
}

}  // namespace objc3::artifacts::frontend
