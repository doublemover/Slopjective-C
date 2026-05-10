#include "artifacts/objc3_frontend_artifact_storage_accessor_abi_manifest.h"

#include <ostream>

#include "ast/objc3_ast_contracts.h"
#include "lower/contracts/object_model_lowering_contracts.h"
#include "lower/contracts/ownership_runtime_accessor_helper_contracts.h"
#include "lower/contracts/runtime_dispatch_abi_contracts.h"
#include "runtime/metadata/runtime_metadata_bootstrap.h"

namespace objc3::artifacts::frontend {

void WriteStorageAccessorRuntimeAbiSurface(
    std::ostream &manifest,
    const Objc3RuntimeBootstrapApiSummary &runtime_bootstrap_api,
    const Objc3RuntimeLinkHostLinkContract &runtime_link_host_link_contract,
    const Objc3PropertySynthesisIvarBindingContract
        &property_synthesis_ivar_binding_contract) {
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
           << "\",\"private_testing_surface_only\":true"
           << ",\"deterministic\":"
           << ((property_synthesis_ivar_binding_contract.deterministic &&
                runtime_link_host_link_contract.deterministic)
                   ? "true"
                   : "false")
           << "},\n";
}

}  // namespace objc3::artifacts::frontend
