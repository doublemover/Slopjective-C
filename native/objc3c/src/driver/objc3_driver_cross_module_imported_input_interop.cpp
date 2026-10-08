#include "driver/objc3_driver_cross_module_imported_input_interop.h"

void PopulateObjc3DriverCrossModuleRuntimeImportedInputInterop(
    Objc3CrossModuleRuntimeLinkPlanImportedInput &imported_input,
    const Objc3ImportedRuntimeModuleSurface &imported_surface) {
  const bool ffi_preservation_active =
      imported_surface.interop_ffi_metadata_interface_preservation_present &&
      (imported_surface.interop_ffi_runtime_import_artifact_ready ||
       imported_surface.interop_ffi_separate_compilation_preservation_ready ||
       imported_surface.interop_ffi_local_foreign_callable_count > 0u ||
       imported_surface.interop_ffi_local_metadata_preservation_sites > 0u ||
       imported_surface.interop_ffi_local_interface_annotation_sites > 0u);
  imported_input.interop_ffi_metadata_interface_preservation_present =
      ffi_preservation_active;
  imported_input.interop_ffi_runtime_import_artifact_ready =
      imported_surface.interop_ffi_runtime_import_artifact_ready;
  imported_input.interop_ffi_separate_compilation_preservation_ready =
      imported_surface.interop_ffi_separate_compilation_preservation_ready;
  imported_input.interop_ffi_deterministic =
      imported_surface.interop_ffi_deterministic;
  imported_input.interop_ffi_contract_id =
      imported_surface.interop_ffi_contract_id;
  imported_input.interop_ffi_source_contract_id =
      imported_surface.interop_ffi_source_contract_id;
  imported_input.interop_ffi_preservation_contract_id =
      imported_surface.interop_ffi_preservation_contract_id;
  imported_input.interop_ffi_replay_key =
      imported_surface.interop_ffi_replay_key;
  imported_input.interop_ffi_lowering_replay_key =
      imported_surface.interop_ffi_lowering_replay_key;
  imported_input.interop_ffi_preservation_replay_key =
      imported_surface.interop_ffi_preservation_replay_key;
  imported_input.interop_ffi_local_foreign_callable_count =
      imported_surface.interop_ffi_local_foreign_callable_count;
  imported_input.interop_ffi_local_metadata_preservation_sites =
      imported_surface.interop_ffi_local_metadata_preservation_sites;
  imported_input.interop_ffi_local_interface_annotation_sites =
      imported_surface.interop_ffi_local_interface_annotation_sites;

  imported_input.interop_header_module_bridge_generation_present =
      imported_surface.interop_header_module_bridge_generation_present;
  imported_input.interop_header_module_bridge_runtime_generation_ready =
      imported_surface.interop_header_module_bridge_runtime_generation_ready;
  imported_input.interop_header_module_bridge_cross_module_packaging_ready =
      imported_surface
          .interop_header_module_bridge_cross_module_packaging_ready;
  imported_input.interop_header_module_bridge_deterministic =
      imported_surface.interop_header_module_bridge_deterministic;
  imported_input.interop_header_module_bridge_contract_id =
      imported_surface.interop_header_module_bridge_contract_id;
  imported_input.interop_header_module_bridge_source_contract_id =
      imported_surface.interop_header_module_bridge_source_contract_id;
  imported_input.interop_header_module_bridge_preservation_contract_id =
      imported_surface.interop_header_module_bridge_preservation_contract_id;
  imported_input.interop_header_module_bridge_replay_key =
      imported_surface.interop_header_module_bridge_replay_key;
  imported_input.interop_header_module_bridge_preservation_replay_key =
      imported_surface.interop_header_module_bridge_preservation_replay_key;
  imported_input.interop_bridge_header_artifact_relative_path =
      imported_surface.interop_bridge_header_artifact_relative_path;
  imported_input.interop_bridge_module_artifact_relative_path =
      imported_surface.interop_bridge_module_artifact_relative_path;
  imported_input.interop_bridge_artifact_relative_path =
      imported_surface.interop_bridge_artifact_relative_path;
  imported_input.interop_header_module_bridge_local_foreign_callable_count =
      imported_surface
          .interop_header_module_bridge_local_foreign_callable_count;

  imported_input.interop_foreign_abi_runtime_closure_present =
      imported_surface.interop_foreign_abi_runtime_closure_present;
  imported_input.interop_foreign_abi_runtime_closure_ready =
      imported_surface.interop_foreign_abi_runtime_closure_ready;
  imported_input.interop_foreign_abi_runtime_closure_deterministic =
      imported_surface.interop_foreign_abi_runtime_closure_deterministic;
  imported_input.interop_foreign_abi_typed_dispatch_ready =
      imported_surface.interop_foreign_abi_typed_dispatch_ready;
  imported_input.interop_foreign_abi_package_runtime_identity_ready =
      imported_surface.interop_foreign_abi_package_runtime_identity_ready;
  imported_input.interop_foreign_abi_bridge_ownership_ready =
      imported_surface.interop_foreign_abi_bridge_ownership_ready;
  imported_input.interop_foreign_abi_runtime_contract_id =
      imported_surface.interop_foreign_abi_runtime_contract_id;
  imported_input.interop_foreign_abi_source_contract_id =
      imported_surface.interop_foreign_abi_source_contract_id;
  imported_input.interop_foreign_abi_package_identity =
      imported_surface.interop_foreign_abi_package_identity;
  imported_input.interop_foreign_abi_runtime_identity =
      imported_surface.interop_foreign_abi_runtime_identity;
  imported_input.interop_foreign_abi_replay_key =
      imported_surface.interop_foreign_abi_replay_key;
  imported_input.interop_foreign_abi_classification_replay_key =
      imported_surface.interop_foreign_abi_classification_replay_key;
  imported_input.interop_foreign_abi_bridge_metadata_replay_key =
      imported_surface.interop_foreign_abi_bridge_metadata_replay_key;
  imported_input.interop_foreign_abi_foreign_surface_count =
      imported_surface.interop_foreign_abi_foreign_surface_count;
  imported_input.interop_foreign_abi_supported_c_abi_surface_count =
      imported_surface.interop_foreign_abi_supported_c_abi_surface_count;
  imported_input.interop_foreign_abi_preserved_swift_metadata_surface_count =
      imported_surface
          .interop_foreign_abi_preserved_swift_metadata_surface_count;
  imported_input.interop_foreign_abi_preserved_cpp_metadata_surface_count =
      imported_surface.interop_foreign_abi_preserved_cpp_metadata_surface_count;
  imported_input.interop_foreign_abi_rejected_surface_count =
      imported_surface.interop_foreign_abi_rejected_surface_count;
  imported_input.interop_foreign_abi_mismatch_negative_case_count =
      imported_surface.interop_foreign_abi_mismatch_negative_case_count;
  imported_input
      .interop_foreign_abi_missing_bridge_ownership_negative_case_count =
      imported_surface
          .interop_foreign_abi_missing_bridge_ownership_negative_case_count;
  imported_input.interop_foreign_abi_unsafe_mixed_image_negative_case_count =
      imported_surface
          .interop_foreign_abi_unsafe_mixed_image_negative_case_count;
  imported_input.interop_foreign_abi_stale_import_negative_case_count =
      imported_surface.interop_foreign_abi_stale_import_negative_case_count;
  imported_input
      .interop_foreign_abi_unsupported_runtime_fallback_negative_case_count =
      imported_surface
          .interop_foreign_abi_unsupported_runtime_fallback_negative_case_count;
}
