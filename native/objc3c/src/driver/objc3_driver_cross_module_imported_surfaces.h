#pragma once

#include <filesystem>
#include <string>
#include <vector>

#include "io/objc3_process.h"
#include "pipeline/objc3_runtime_import_surface.h"

struct Objc3DriverCrossModuleImportedSurfaces {
  std::vector<Objc3ImportedRuntimeModuleSurface> surfaces;
  std::vector<Objc3ImportedRuntimeModulePackagingPeerArtifacts> peer_artifacts;
};

bool TryLoadObjc3DriverCrossModuleImportedSurfaces(
    const std::vector<std::filesystem::path> &import_paths,
    Objc3DriverCrossModuleImportedSurfaces &loaded_imports,
    std::string &error);
