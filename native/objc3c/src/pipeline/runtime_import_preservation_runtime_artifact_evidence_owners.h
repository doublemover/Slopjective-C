#pragma once

#include <string>

#include "pipeline/objc3_runtime_import_surface.h"
#include "pipeline/runtime_import_json_helpers.h"

namespace objc3c::pipeline::runtime_import_preservation {

bool PopulateImportedRuntimeBlockOwnershipArtifactPreservation(
    const RuntimeImportJsonValue::Object &root,
    Objc3ImportedRuntimeModuleSurface &surface,
    std::string &error);

bool PopulateImportedRuntimeStorageReflectionArtifactPreservation(
    const RuntimeImportJsonValue::Object &root,
    Objc3ImportedRuntimeModuleSurface &surface,
    std::string &error);

}  // namespace objc3c::pipeline::runtime_import_preservation
