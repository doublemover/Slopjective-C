#include "pipeline/runtime_import_packaging_peer_artifacts.h"

#include <vector>

#include "lower/objc3_lowering_contract.h"
#include "pipeline/objc3_runtime_import_surface.h"
#include "pipeline/runtime_import_manifest_preservation.h"

namespace objc3c::pipeline {

bool PopulateImportedRuntimeRegistrationManifestPeerArtifacts(
    const RuntimeImportJsonValue::Object &root,
    Objc3ImportedRuntimeModulePackagingPeerArtifacts &artifacts,
    std::string &error) {
  std::string contract_id;
  bool ready_for_runtime_bootstrap_enforcement = false;
  bool ready_for_live_registration_discovery_replay = false;
  bool ready_for_live_restart_hardening = false;
  if (!ReadStringMember(root, "contract_id", contract_id, error) ||
      !ReadStringMember(root, "runtime_support_library_archive_relative_path",
                        artifacts.runtime_support_library_archive_relative_path,
                        error) ||
      !ReadStringMember(root, "translation_unit_identity_model",
                        artifacts.translation_unit_identity_model, error) ||
      !ReadStringMember(root, "translation_unit_identity_key",
                        artifacts.translation_unit_identity_key, error) ||
      !ReadStringMember(root, "object_format", artifacts.object_format, error) ||
      !ReadUnsignedMember(root, "translation_unit_registration_order_ordinal",
                          artifacts.translation_unit_registration_order_ordinal,
                          error) ||
      !ReadUnsignedMember(root, "class_descriptor_count",
                          artifacts.class_descriptor_count, error) ||
      !ReadUnsignedMember(root, "protocol_descriptor_count",
                          artifacts.protocol_descriptor_count, error) ||
      !ReadUnsignedMember(root, "category_descriptor_count",
                          artifacts.category_descriptor_count, error) ||
      !ReadUnsignedMember(root, "property_descriptor_count",
                          artifacts.property_descriptor_count, error) ||
      !ReadUnsignedMember(root, "ivar_descriptor_count",
                          artifacts.ivar_descriptor_count, error) ||
      !ReadUnsignedMember(root, "total_descriptor_count",
                          artifacts.total_descriptor_count, error) ||
      !ReadStringArrayMember(root, "driver_linker_flags",
                             artifacts.driver_linker_flags, error) ||
      !ReadStringMember(root, "bootstrap_live_registration_contract_id",
                        artifacts.bootstrap_live_registration_contract_id,
                        error) ||
      !ReadStringMember(root, "bootstrap_live_restart_hardening_contract_id",
                        artifacts.bootstrap_live_restart_hardening_contract_id,
                        error) ||
      !ReadStringMember(root, "bootstrap_live_replay_registered_images_symbol",
                        artifacts.bootstrap_live_replay_registered_images_symbol,
                        error) ||
      !ReadStringMember(root, "bootstrap_live_reset_replay_state_snapshot_symbol",
                        artifacts.bootstrap_live_reset_replay_state_snapshot_symbol,
                        error) ||
      !ReadStringMember(root, "bootstrap_live_restart_reset_for_testing_symbol",
                        artifacts.bootstrap_live_restart_reset_for_testing_symbol,
                        error) ||
      !ReadStringMember(root, "bootstrap_live_restart_replay_registered_images_symbol",
                        artifacts.bootstrap_live_restart_replay_registered_images_symbol,
                        error) ||
      !ReadStringMember(root, "bootstrap_live_restart_reset_replay_state_snapshot_symbol",
                        artifacts.bootstrap_live_restart_reset_replay_state_snapshot_symbol,
                        error) ||
      !ReadBoolMember(root, "ready_for_runtime_bootstrap_enforcement",
                      ready_for_runtime_bootstrap_enforcement, error) ||
      !ReadBoolMember(root, "ready_for_live_registration_discovery_replay",
                      ready_for_live_registration_discovery_replay, error) ||
      !ReadBoolMember(root, "ready_for_live_restart_hardening",
                      ready_for_live_restart_hardening, error)) {
    return false;
  }
  if (contract_id !=
      kObjc3RuntimeTranslationUnitRegistrationManifestContractId) {
    error = "unexpected runtime registration manifest contract id";
    return false;
  }
  if (!ValidateImportedRuntimeRegistrationManifestReadiness(
          ready_for_runtime_bootstrap_enforcement,
          ready_for_live_registration_discovery_replay,
          ready_for_live_restart_hardening, error) ||
      !ValidateImportedRuntimeRegistrationManifestDescriptorInventory(
          artifacts, error) ||
      !ValidateImportedRuntimeRegistrationManifestLinkerFlags(
          artifacts.driver_linker_flags, error)) {
    return false;
  }
  artifacts.ready_for_live_registration_discovery_replay =
      ready_for_live_registration_discovery_replay;
  artifacts.ready_for_live_restart_hardening =
      ready_for_live_restart_hardening;
  return true;
}

bool ValidateImportedRuntimeDiscoveryPeerArtifacts(
    const RuntimeImportJsonValue::Object &root,
    const Objc3ImportedRuntimeModulePackagingPeerArtifacts &artifacts,
    std::string &object_artifact_relative_path, std::string &error) {
  std::string contract_id;
  std::string translation_unit_identity_model;
  std::string translation_unit_identity_key;
  std::string object_format;
  std::vector<std::string> driver_linker_flags;
  if (!ReadStringMember(root, "contract_id", contract_id, error) ||
      !ReadStringMember(root, "object_artifact",
                        object_artifact_relative_path, error) ||
      !ReadStringMember(root, "translation_unit_identity_model",
                        translation_unit_identity_model, error) ||
      !ReadStringMember(root, "translation_unit_identity_key",
                        translation_unit_identity_key, error) ||
      !ReadStringMember(root, "object_format", object_format, error) ||
      !ReadStringArrayMember(root, "driver_linker_flags", driver_linker_flags,
                             error)) {
    return false;
  }
  if (contract_id != kObjc3RuntimeLinkerRetentionContractId) {
    error = "unexpected runtime metadata discovery contract id";
    return false;
  }
  if (translation_unit_identity_model !=
      artifacts.translation_unit_identity_model) {
    error =
        "runtime metadata discovery translation-unit identity model mismatch";
    return false;
  }
  if (translation_unit_identity_key != artifacts.translation_unit_identity_key) {
    error = "runtime metadata discovery translation-unit identity key mismatch";
    return false;
  }
  if (object_format != artifacts.object_format) {
    error = "runtime metadata discovery object format mismatch";
    return false;
  }
  if (driver_linker_flags != artifacts.driver_linker_flags) {
    error = "runtime metadata discovery driver linker flags mismatch";
    return false;
  }
  return true;
}

}  // namespace objc3c::pipeline
