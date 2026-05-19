#include "driver/objc3_driver_cross_module_imported_surfaces.h"

#include <filesystem>
#include <string>
#include <utility>

#include "driver/objc3_driver_cross_module_imported_input.h"

namespace fs = std::filesystem;

bool TryLoadObjc3DriverCrossModuleImportedSurfaces(
    const std::vector<std::filesystem::path> &import_paths,
    Objc3DriverCrossModuleImportedSurfaces &loaded_imports,
    std::string &error) {
  loaded_imports = Objc3DriverCrossModuleImportedSurfaces{};
  loaded_imports.surfaces.reserve(import_paths.size());
  loaded_imports.peer_artifacts.reserve(import_paths.size());

  for (const auto &import_path : import_paths) {
    Objc3ImportedRuntimeModuleSurface imported_surface;
    const fs::path absolute_import_path =
        fs::absolute(import_path).lexically_normal();
    if (!TryLoadObjc3ImportedRuntimeModuleSurface(
            absolute_import_path, imported_surface, error)) {
      return false;
    }

    Objc3ImportedRuntimeModulePackagingPeerArtifacts peer_artifacts;
    if (!TryLoadObjc3ImportedRuntimeModulePackagingPeerArtifacts(
            imported_surface, peer_artifacts, error)) {
      return false;
    }
    loaded_imports.surfaces.push_back(std::move(imported_surface));
    loaded_imports.peer_artifacts.push_back(std::move(peer_artifacts));
  }
  return true;
}
