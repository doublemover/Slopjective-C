#include "driver/objc3_driver_cross_module_imported_input_storage.h"

void PopulateObjc3DriverCrossModuleRuntimeImportedInputStorageReflection(
    Objc3CrossModuleRuntimeLinkPlanImportedInput &imported_input,
    const Objc3ImportedRuntimeModuleSurface &imported_surface) {
  imported_input.storage_reflection_artifact_preservation_present =
      imported_surface.storage_reflection_artifact_preservation_present;
  imported_input.storage_reflection_runtime_import_artifact_ready =
      imported_surface.storage_reflection_runtime_import_artifact_ready;
  imported_input.storage_reflection_separate_compilation_preservation_ready =
      imported_surface.storage_reflection_separate_compilation_preservation_ready;
  imported_input.storage_reflection_deterministic =
      imported_surface.storage_reflection_deterministic;
  imported_input.storage_reflection_contract_id =
      imported_surface.storage_reflection_contract_id;
  imported_input.storage_reflection_source_contract_id =
      imported_surface.storage_reflection_source_contract_id;
  imported_input
      .storage_reflection_dispatch_and_synthesized_accessor_lowering_surface_contract_id =
      imported_surface
          .storage_reflection_dispatch_and_synthesized_accessor_lowering_surface_contract_id;
  imported_input
      .storage_reflection_executable_property_accessor_layout_lowering_contract_id =
      imported_surface
          .storage_reflection_executable_property_accessor_layout_lowering_contract_id;
  imported_input
      .storage_reflection_executable_ivar_layout_emission_contract_id =
      imported_surface
          .storage_reflection_executable_ivar_layout_emission_contract_id;
  imported_input
      .storage_reflection_executable_synthesized_accessor_property_lowering_contract_id =
      imported_surface
          .storage_reflection_executable_synthesized_accessor_property_lowering_contract_id;
  imported_input.storage_reflection_replay_key =
      imported_surface.storage_reflection_replay_key;
  imported_input.storage_reflection_local_property_descriptor_count =
      imported_surface.storage_reflection_local_property_descriptor_count;
  imported_input.storage_reflection_local_ivar_descriptor_count =
      imported_surface.storage_reflection_local_ivar_descriptor_count;
  imported_input.storage_reflection_implementation_owned_property_entries =
      imported_surface.storage_reflection_implementation_owned_property_entries;
  imported_input.storage_reflection_synthesized_accessor_owner_entries =
      imported_surface.storage_reflection_synthesized_accessor_owner_entries;
  imported_input.storage_reflection_synthesized_getter_entries =
      imported_surface.storage_reflection_synthesized_getter_entries;
  imported_input.storage_reflection_synthesized_setter_entries =
      imported_surface.storage_reflection_synthesized_setter_entries;
  imported_input.storage_reflection_synthesized_accessor_entries =
      imported_surface.storage_reflection_synthesized_accessor_entries;
  imported_input.storage_reflection_current_property_read_entries =
      imported_surface.storage_reflection_current_property_read_entries;
  imported_input.storage_reflection_current_property_write_entries =
      imported_surface.storage_reflection_current_property_write_entries;
  imported_input.storage_reflection_current_property_exchange_entries =
      imported_surface.storage_reflection_current_property_exchange_entries;
  imported_input.storage_reflection_weak_current_property_load_entries =
      imported_surface.storage_reflection_weak_current_property_load_entries;
  imported_input.storage_reflection_weak_current_property_store_entries =
      imported_surface.storage_reflection_weak_current_property_store_entries;
  imported_input.storage_reflection_ivar_layout_entries =
      imported_surface.storage_reflection_ivar_layout_entries;
  imported_input.storage_reflection_ivar_layout_owner_entries =
      imported_surface.storage_reflection_ivar_layout_owner_entries;
}
