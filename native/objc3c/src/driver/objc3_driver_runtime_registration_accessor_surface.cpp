#include "driver/objc3_driver_runtime_registration_accessor_surface.h"

#include "ast/objc3_ast.h"
#include "io/objc3_process.h"
#include "lower/contracts/ownership_runtime_accessor_helper_contracts.h"
#include "lower/contracts/ownership_runtime_memory_management_contracts.h"
#include "runtime/metadata/runtime_metadata_bootstrap.h"
#include "runtime/metadata/selector_metadata.h"

namespace {

constexpr const char *kObjc3DriverDispatchAccessorBoundaryModel =
    "public-dispatch-entrypoint-plus-private-testing-snapshot-and-property-accessor-surface";
constexpr const char *kObjc3DriverStorageAccessorBoundaryModel =
    "private-bootstrap-internal-property-accessor-and-reflection-snapshot-surface-without-public-header-widening";

}  // namespace

void PopulateObjc3DriverRuntimeRegistrationAccessorSurfaceInputs(
    Objc3RuntimeTranslationUnitRegistrationManifestArtifactInputs &inputs,
    const Objc3RuntimeBootstrapApiSummary &runtime_bootstrap_api_summary) {
  inputs.dispatch_accessor_runtime_abi_contract_id =
      "objc3c.runtime.dispatch_accessor.abi.surface.v1";
  inputs.dispatch_accessor_runtime_abi_boundary_model =
      kObjc3DriverDispatchAccessorBoundaryModel;
  inputs.dispatch_accessor_public_header_path =
      "native/objc3c/src/runtime/public/objc3_runtime_api.h";
  inputs.dispatch_accessor_private_header_path =
      "native/objc3c/src/runtime/objc3_runtime_bootstrap_internal.h";
  inputs.dispatch_accessor_runtime_dispatch_symbol =
      runtime_bootstrap_api_summary.dispatch_entrypoint_symbol;
  inputs.dispatch_accessor_dispatch_state_snapshot_symbol =
      "objc3_runtime_copy_dispatch_state_for_testing";
  inputs.dispatch_accessor_method_cache_state_snapshot_symbol =
      "objc3_runtime_copy_method_cache_state_for_testing";
  inputs.dispatch_accessor_method_cache_entry_snapshot_symbol =
      "objc3_runtime_copy_method_cache_entry_for_testing";
  inputs.dispatch_accessor_property_registry_state_snapshot_symbol =
      "objc3_runtime_copy_property_registry_state_for_testing";
  inputs.dispatch_accessor_property_entry_snapshot_symbol =
      "objc3_runtime_copy_property_entry_for_testing";
  inputs.dispatch_accessor_arc_debug_state_snapshot_symbol =
      "objc3_runtime_copy_arc_debug_state_for_testing";
  inputs.dispatch_accessor_current_property_read_symbol =
      kObjc3RuntimeReadCurrentPropertyI32Symbol;
  inputs.dispatch_accessor_current_property_write_symbol =
      kObjc3RuntimeWriteCurrentPropertyI32Symbol;
  inputs.dispatch_accessor_current_property_exchange_symbol =
      kObjc3RuntimeExchangeCurrentPropertyI32Symbol;
  inputs.dispatch_accessor_bind_current_property_context_symbol =
      "objc3_runtime_bind_current_property_context_for_testing";
  inputs.dispatch_accessor_clear_current_property_context_symbol =
      "objc3_runtime_clear_current_property_context_for_testing";
  inputs.dispatch_accessor_weak_current_property_load_symbol =
      kObjc3RuntimeLoadWeakCurrentPropertyI32Symbol;
  inputs.dispatch_accessor_weak_current_property_store_symbol =
      kObjc3RuntimeStoreWeakCurrentPropertyI32Symbol;
  inputs.dispatch_accessor_retain_symbol = kObjc3RuntimeRetainI32Symbol;
  inputs.dispatch_accessor_release_symbol = kObjc3RuntimeReleaseI32Symbol;
  inputs.dispatch_accessor_autorelease_symbol =
      kObjc3RuntimeAutoreleaseI32Symbol;
  inputs.dispatch_accessor_private_testing_surface_only = true;
  inputs.dispatch_accessor_deterministic = true;

  inputs.storage_accessor_runtime_abi_contract_id =
      kObjc3RuntimeStorageAccessorAbiSurfaceContractId;
  inputs.storage_accessor_runtime_abi_boundary_model =
      kObjc3DriverStorageAccessorBoundaryModel;
  inputs.storage_accessor_public_header_path =
      "native/objc3c/src/runtime/public/objc3_runtime_api.h";
  inputs.storage_accessor_private_header_path =
      "native/objc3c/src/runtime/objc3_runtime_bootstrap_internal.h";
  inputs.storage_accessor_property_registry_state_snapshot_symbol =
      "objc3_runtime_copy_property_registry_state_for_testing";
  inputs.storage_accessor_property_entry_snapshot_symbol =
      "objc3_runtime_copy_property_entry_for_testing";
  inputs.storage_accessor_current_property_read_symbol =
      kObjc3RuntimeReadCurrentPropertyI32Symbol;
  inputs.storage_accessor_current_property_write_symbol =
      kObjc3RuntimeWriteCurrentPropertyI32Symbol;
  inputs.storage_accessor_current_property_exchange_symbol =
      kObjc3RuntimeExchangeCurrentPropertyI32Symbol;
  inputs.storage_accessor_bind_current_property_context_symbol =
      "objc3_runtime_bind_current_property_context_for_testing";
  inputs.storage_accessor_clear_current_property_context_symbol =
      "objc3_runtime_clear_current_property_context_for_testing";
  inputs.storage_accessor_weak_current_property_load_symbol =
      kObjc3RuntimeLoadWeakCurrentPropertyI32Symbol;
  inputs.storage_accessor_weak_current_property_store_symbol =
      kObjc3RuntimeStoreWeakCurrentPropertyI32Symbol;
  inputs.storage_accessor_private_testing_surface_only = true;
  inputs.storage_accessor_deterministic = true;
}
