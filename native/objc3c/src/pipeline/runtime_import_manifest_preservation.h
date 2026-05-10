#pragma once

#include <string>
#include <vector>

#include "pipeline/objc3_runtime_import_surface.h"

namespace objc3c::pipeline {

bool ValidateImportedRuntimeRegistrationManifestReadiness(
    bool ready_for_runtime_bootstrap_enforcement,
    bool ready_for_live_registration_discovery_replay,
    bool ready_for_live_restart_hardening,
    std::string &error);

bool ValidateImportedRuntimeRegistrationManifestDescriptorInventory(
    const Objc3ImportedRuntimeModulePackagingPeerArtifacts &artifacts,
    std::string &error);

bool ValidateImportedRuntimeRegistrationManifestLinkerFlags(
    const std::vector<std::string> &driver_linker_flags,
    std::string &error);

}  // namespace objc3c::pipeline
