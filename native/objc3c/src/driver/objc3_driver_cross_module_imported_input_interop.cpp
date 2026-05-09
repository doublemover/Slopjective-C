#include "driver/objc3_driver_cross_module_imported_input_interop.h"

void PopulateObjc3DriverCrossModuleRuntimeImportedInputInterop(
    Objc3CrossModuleRuntimeLinkPlanImportedInput &imported_input,
    const Objc3ImportedRuntimeModuleSurface &imported_surface) {
  imported_input.interop_ffi_metadata_interface_preservation_present =
      imported_surface.interop_ffi_metadata_interface_preservation_present;
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
}
