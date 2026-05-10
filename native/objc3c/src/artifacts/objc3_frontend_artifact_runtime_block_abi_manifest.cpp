#include "artifacts/objc3_frontend_artifact_runtime_block_abi_manifest.h"

#include <ostream>

#include "ast/objc3_ast_contracts.h"
#include "lower/contracts/block_runtime_helper_contracts.h"
#include "lower/contracts/ownership_runtime_accessor_helper_contracts.h"
#include "lower/contracts/ownership_runtime_memory_management_contracts.h"
#include "runtime/metadata/runtime_metadata_bootstrap.h"
#include "runtime/metadata/selector_metadata.h"

namespace objc3::artifacts::frontend {

void WriteRuntimeBlockArcRuntimeAbiSurface(
    std::ostream &manifest,
    const Objc3RuntimeBootstrapApiSummary &runtime_bootstrap_api) {
  manifest << "  \"runtime_block_arc_runtime_abi_surface\":{\"contract_id\":\""
           << kObjc3RuntimeBlockArcRuntimeAbiSurfaceContractId
           << "\",\"public_header_path\":\""
           << runtime_bootstrap_api.public_header_path
           << "\",\"internal_header_path\":\""
           << kObjc3RuntimeBootstrapInternalHeaderPath
           << "\",\"block_arc_unified_source_surface_contract_id\":\""
           << kObjc3RuntimeBlockArcUnifiedSourceSurfaceContractId
           << "\",\"block_arc_lowering_helper_surface_contract_id\":\""
           << kObjc3RuntimeBlockArcLoweringHelperSurfaceContractId
           << "\",\"public_runtime_abi_boundary\":[\""
           << kObjc3RuntimeSupportLibraryRegisterImageSymbol << "\",\""
           << kObjc3RuntimeSupportLibraryLookupSelectorSymbol << "\",\""
           << kObjc3RuntimeSupportLibraryDispatchI32Symbol << "\",\""
           << kObjc3RuntimeSupportLibraryResetForTestingSymbol << "\"]"
           << ",\"private_block_arc_runtime_abi_boundary\":[\""
           << kObjc3RuntimePromoteBlockI32Symbol << "\",\""
           << kObjc3RuntimeInvokeBlockI32Symbol << "\",\""
           << kObjc3RuntimeRetainI32Symbol << "\",\""
           << kObjc3RuntimeReleaseI32Symbol << "\",\""
           << kObjc3RuntimeAutoreleaseI32Symbol << "\",\""
           << kObjc3RuntimePushAutoreleasepoolScopeSymbol << "\",\""
           << kObjc3RuntimePopAutoreleasepoolScopeSymbol << "\",\""
           << kObjc3RuntimeReadCurrentPropertyI32Symbol << "\",\""
           << kObjc3RuntimeWriteCurrentPropertyI32Symbol << "\",\""
           << kObjc3RuntimeExchangeCurrentPropertyI32Symbol << "\",\""
           << "objc3_runtime_bind_current_property_context_for_testing"
           << "\",\"objc3_runtime_clear_current_property_context_for_testing"
           << "\",\"" << kObjc3RuntimeLoadWeakCurrentPropertyI32Symbol << "\",\""
           << kObjc3RuntimeStoreWeakCurrentPropertyI32Symbol << "\",\""
           << "objc3_runtime_copy_arc_debug_state_for_testing"
           << "\",\"objc3_runtime_copy_block_arc_runtime_abi_snapshot_for_testing\"]"
           << ",\"block_arc_runtime_abi_snapshot_symbol\":\"objc3_runtime_copy_block_arc_runtime_abi_snapshot_for_testing\""
           << ",\"arc_debug_state_snapshot_symbol\":\"objc3_runtime_copy_arc_debug_state_for_testing\""
           << ",\"runtime_abi_boundary_model\":\""
           << kObjc3RuntimeBlockArcRuntimeAbiBoundaryModel
           << "\",\"block_runtime_model\":\""
           << kObjc3RuntimeBlockArcRuntimeAbiBlockModel
           << "\",\"arc_runtime_model\":\""
           << kObjc3RuntimeBlockArcRuntimeAbiArcModel
           << "\",\"fail_closed_model\":\""
           << kObjc3RuntimeBlockArcRuntimeAbiFailClosedModel
           << "\",\"authoritative_probe_path\":\"tests/tooling/runtime/block_arc_runtime_abi_probe.cpp\""
           << ",\"requires_coupled_registration_manifest\":true"
           << ",\"requires_real_compile_output\":true"
           << ",\"requires_linked_runtime_probe\":true"
           << "},\n";
}

}  // namespace objc3::artifacts::frontend
