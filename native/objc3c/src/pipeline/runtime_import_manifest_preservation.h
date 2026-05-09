#pragma once

#include <string>
#include <vector>

#include "pipeline/objc3_runtime_import_surface.h"

namespace objc3c::pipeline {

inline bool ValidateImportedRuntimeRegistrationManifestReadiness(
    bool ready_for_runtime_bootstrap_enforcement,
    bool ready_for_live_registration_discovery_replay,
    bool ready_for_live_restart_hardening,
    std::string &error) {
  if (!ready_for_runtime_bootstrap_enforcement) {
    error =
        "runtime registration manifest is not ready for runtime bootstrap enforcement";
    return false;
  }
  if (!ready_for_live_registration_discovery_replay) {
    error =
        "runtime registration manifest is not ready for live registration discovery replay";
    return false;
  }
  if (!ready_for_live_restart_hardening) {
    error =
        "runtime registration manifest is not ready for live restart hardening";
    return false;
  }
  return true;
}

inline bool ValidateImportedRuntimeRegistrationManifestDescriptorInventory(
    const Objc3ImportedRuntimeModulePackagingPeerArtifacts &artifacts,
    std::string &error) {
  if (artifacts.total_descriptor_count !=
      artifacts.class_descriptor_count + artifacts.protocol_descriptor_count +
          artifacts.category_descriptor_count +
          artifacts.property_descriptor_count + artifacts.ivar_descriptor_count) {
    error =
        "runtime registration manifest descriptor counts are internally inconsistent";
    return false;
  }
  return true;
}

inline bool ValidateImportedRuntimeRegistrationManifestLinkerFlags(
    const std::vector<std::string> &driver_linker_flags,
    std::string &error) {
  for (const auto &flag : driver_linker_flags) {
    if (flag.empty()) {
      error =
          "runtime registration manifest contains an empty driver linker flag";
      return false;
    }
  }
  return true;
}

}  // namespace objc3c::pipeline
