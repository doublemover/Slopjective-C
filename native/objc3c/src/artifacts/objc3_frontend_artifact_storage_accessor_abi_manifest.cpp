#include "artifacts/objc3_frontend_artifact_storage_accessor_abi_manifest.h"

#include <ostream>

#include "artifacts/objc3_frontend_artifact_storage_accessor_manifest_contracts.h"
#include "ast/objc3_ast_contracts_runtime_bootstrap_support_registrar_reset.h"
#include "ast/objc3_ast_contracts_source_property_metadata.h"
#include "runtime/metadata/runtime_metadata_bootstrap.h"

namespace objc3::artifacts::frontend {

void WriteStorageAccessorRuntimeAbiSurface(
    std::ostream &manifest,
    const Objc3RuntimeBootstrapApiSummary &runtime_bootstrap_api,
    const Objc3StorageAccessorRuntimeAbiFields
        &storage_accessor_runtime_abi_fields) {
  manifest << "  \"storage_accessor_runtime_abi_surface\":{\"contract_id\":\""
           << kObjc3RuntimeStorageAccessorAbiSurfaceContractId
           << "\",\"abi_boundary_model\":\"private-bootstrap-internal-property-helper-and-reflection-snapshot-surface-without-public-header-widening\""
           << ",\"public_header_path\":\""
           << runtime_bootstrap_api.public_header_path
           << "\",\"private_header_path\":\""
           << kObjc3RuntimeBootstrapInternalHeaderPath
           << "\",\"property_registry_state_snapshot_symbol\":\"objc3_runtime_copy_property_registry_state_for_testing\""
           << ",\"property_entry_snapshot_symbol\":\"objc3_runtime_copy_property_entry_for_testing\""
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
           << "\",\"private_testing_surface_only\":true"
           << ",\"deterministic\":"
           << (storage_accessor_runtime_abi_fields.deterministic ? "true"
                                                                 : "false")
           << "},\n";
}

}  // namespace objc3::artifacts::frontend
