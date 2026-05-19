#include "driver/objc3_driver_cross_module_imported_input_core.h"

Objc3CrossModuleRuntimeLinkPlanImportedInput
BuildObjc3DriverCrossModuleRuntimeImportedInputCore(
    const Objc3ImportedRuntimeModuleSurface &imported_surface,
    const Objc3ImportedRuntimeModulePackagingPeerArtifacts &peer_artifacts) {
  Objc3CrossModuleRuntimeLinkPlanImportedInput imported_input;
  imported_input.module_name =
      imported_surface.frontend_closure_summary.module_name;
  imported_input.import_surface_artifact_path =
      imported_surface.source_path.generic_string();
  imported_input.registration_manifest_artifact_path =
      peer_artifacts.registration_manifest_path.generic_string();
  imported_input.object_artifact_path =
      peer_artifacts.object_artifact_path.generic_string();
  imported_input.discovery_artifact_path =
      peer_artifacts.discovery_artifact_path.generic_string();
  imported_input.linker_response_artifact_path =
      peer_artifacts.linker_response_artifact_path.generic_string();
  imported_input.translation_unit_identity_model =
      peer_artifacts.translation_unit_identity_model;
  imported_input.translation_unit_identity_key =
      peer_artifacts.translation_unit_identity_key;
  imported_input.object_format = peer_artifacts.object_format;
  imported_input.runtime_support_library_archive_relative_path =
      peer_artifacts.runtime_support_library_archive_relative_path;
  imported_input.translation_unit_registration_order_ordinal =
      peer_artifacts.translation_unit_registration_order_ordinal;
  imported_input.class_descriptor_count = peer_artifacts.class_descriptor_count;
  imported_input.protocol_descriptor_count =
      peer_artifacts.protocol_descriptor_count;
  imported_input.category_descriptor_count =
      peer_artifacts.category_descriptor_count;
  imported_input.property_descriptor_count =
      peer_artifacts.property_descriptor_count;
  imported_input.ivar_descriptor_count = peer_artifacts.ivar_descriptor_count;
  imported_input.total_descriptor_count = peer_artifacts.total_descriptor_count;
  imported_input.driver_linker_flags = peer_artifacts.driver_linker_flags;
  imported_input.ready_for_live_registration_discovery_replay =
      peer_artifacts.ready_for_live_registration_discovery_replay;
  imported_input.ready_for_live_restart_hardening =
      peer_artifacts.ready_for_live_restart_hardening;
  imported_input.bootstrap_live_registration_contract_id =
      peer_artifacts.bootstrap_live_registration_contract_id;
  imported_input.bootstrap_live_restart_hardening_contract_id =
      peer_artifacts.bootstrap_live_restart_hardening_contract_id;
  imported_input.bootstrap_live_replay_registered_images_symbol =
      peer_artifacts.bootstrap_live_replay_registered_images_symbol;
  imported_input.bootstrap_live_reset_replay_state_snapshot_symbol =
      peer_artifacts.bootstrap_live_reset_replay_state_snapshot_symbol;
  imported_input.bootstrap_live_restart_reset_for_testing_symbol =
      peer_artifacts.bootstrap_live_restart_reset_for_testing_symbol;
  imported_input.bootstrap_live_restart_replay_registered_images_symbol =
      peer_artifacts.bootstrap_live_restart_replay_registered_images_symbol;
  imported_input.bootstrap_live_restart_reset_replay_state_snapshot_symbol =
      peer_artifacts.bootstrap_live_restart_reset_replay_state_snapshot_symbol;
  return imported_input;
}
