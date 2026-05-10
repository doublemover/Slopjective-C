#include "artifacts/objc3_frontend_artifact_runtime_block_abi_manifest.h"

#include <ostream>

#include "ast/objc3_ast_contracts.h"
#include "runtime/metadata/runtime_metadata_bootstrap.h"
#include "runtime/metadata/selector_metadata.h"

namespace objc3::artifacts::frontend {
namespace {

inline constexpr const char *kArtifactRuntimePromoteBlockI32Symbol =
    "objc3_runtime_promote_block_i32";
inline constexpr const char *kArtifactRuntimeInvokeBlockI32Symbol =
    "objc3_runtime_invoke_block_i32";
inline constexpr const char *kArtifactRuntimeRetainI32Symbol =
    "objc3_runtime_retain_i32";
inline constexpr const char *kArtifactRuntimeReleaseI32Symbol =
    "objc3_runtime_release_i32";
inline constexpr const char *kArtifactRuntimeAutoreleaseI32Symbol =
    "objc3_runtime_autorelease_i32";
inline constexpr const char *kArtifactRuntimePushAutoreleasepoolScopeSymbol =
    "objc3_runtime_push_autoreleasepool_scope";
inline constexpr const char *kArtifactRuntimePopAutoreleasepoolScopeSymbol =
    "objc3_runtime_pop_autoreleasepool_scope";
inline constexpr const char *kArtifactRuntimeReadCurrentPropertyI32Symbol =
    "objc3_runtime_read_current_property_i32";
inline constexpr const char *kArtifactRuntimeWriteCurrentPropertyI32Symbol =
    "objc3_runtime_write_current_property_i32";
inline constexpr const char *kArtifactRuntimeExchangeCurrentPropertyI32Symbol =
    "objc3_runtime_exchange_current_property_i32";
inline constexpr const char *kArtifactRuntimeLoadWeakCurrentPropertyI32Symbol =
    "objc3_runtime_load_weak_current_property_i32";
inline constexpr const char *kArtifactRuntimeStoreWeakCurrentPropertyI32Symbol =
    "objc3_runtime_store_weak_current_property_i32";

}  // namespace

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
           << kArtifactRuntimePromoteBlockI32Symbol << "\",\""
           << kArtifactRuntimeInvokeBlockI32Symbol << "\",\""
           << kArtifactRuntimeRetainI32Symbol << "\",\""
           << kArtifactRuntimeReleaseI32Symbol << "\",\""
           << kArtifactRuntimeAutoreleaseI32Symbol << "\",\""
           << kArtifactRuntimePushAutoreleasepoolScopeSymbol << "\",\""
           << kArtifactRuntimePopAutoreleasepoolScopeSymbol << "\",\""
           << kArtifactRuntimeReadCurrentPropertyI32Symbol << "\",\""
           << kArtifactRuntimeWriteCurrentPropertyI32Symbol << "\",\""
           << kArtifactRuntimeExchangeCurrentPropertyI32Symbol << "\",\""
           << "objc3_runtime_bind_current_property_context_for_testing"
           << "\",\"objc3_runtime_clear_current_property_context_for_testing"
           << "\",\"" << kArtifactRuntimeLoadWeakCurrentPropertyI32Symbol
           << "\",\"" << kArtifactRuntimeStoreWeakCurrentPropertyI32Symbol
           << "\",\""
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
