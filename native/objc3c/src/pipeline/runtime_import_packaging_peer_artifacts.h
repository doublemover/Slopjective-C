#pragma once

#include <string>

#include "pipeline/runtime_import_json_helpers.h"

struct Objc3ImportedRuntimeModulePackagingPeerArtifacts;

namespace objc3c::pipeline {

bool PopulateImportedRuntimeRegistrationManifestPeerArtifacts(
    const RuntimeImportJsonValue::Object &root,
    Objc3ImportedRuntimeModulePackagingPeerArtifacts &artifacts,
    std::string &error);

bool ValidateImportedRuntimeDiscoveryPeerArtifacts(
    const RuntimeImportJsonValue::Object &root,
    const Objc3ImportedRuntimeModulePackagingPeerArtifacts &artifacts,
    std::string &object_artifact_relative_path,
    std::string &error);

}  // namespace objc3c::pipeline
