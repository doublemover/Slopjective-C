#include "pipeline/objc3_runtime_import_surface.h"

bool IsReadyObjc3ImportedRuntimeModulePackagingLinkPlan(
    const Objc3ImportedRuntimeModulePackagingPeerArtifacts &artifacts) {
  const bool live_restart_replay_symbols_ready =
      !artifacts.bootstrap_live_restart_replay_registered_images_symbol
           .empty() &&
      !artifacts.bootstrap_live_restart_reset_replay_state_snapshot_symbol
           .empty();
  const bool bootstrap_symbols_ready =
      !artifacts.bootstrap_live_registration_contract_id.empty() &&
      !artifacts.bootstrap_live_restart_hardening_contract_id.empty() &&
      !artifacts.bootstrap_live_replay_registered_images_symbol.empty() &&
      !artifacts.bootstrap_live_reset_replay_state_snapshot_symbol.empty() &&
      !artifacts.bootstrap_live_restart_reset_for_testing_symbol.empty() &&
      live_restart_replay_symbols_ready;
  return !artifacts.registration_manifest_path.empty() &&
         !artifacts.object_artifact_path.empty() &&
         !artifacts.discovery_artifact_path.empty() &&
         !artifacts.linker_response_artifact_path.empty() &&
         !artifacts.runtime_support_library_archive_relative_path.empty() &&
         !artifacts.translation_unit_identity_model.empty() &&
         !artifacts.translation_unit_identity_key.empty() &&
         !artifacts.object_format.empty() &&
         artifacts.ready_for_live_registration_discovery_replay &&
         artifacts.ready_for_live_restart_hardening && bootstrap_symbols_ready;
}
