#include "pipeline/runtime_import_preservation_owners.h"

namespace objc3c::pipeline::runtime_import_preservation {

bool ParseImportedRuntimeModuleSurface(
    const RuntimeImportJsonValue::Object &root,
    Objc3ImportedRuntimeModuleSurface &surface,
    std::string &error) {
  if (!PopulatePreservationEvidence(root, surface, error)) {
    return false;
  }
  return PopulateSerializedRuntimeMetadataReuse(root, surface, error);
}

bool PublishImportedRuntimeModuleSurfaceReadiness(
    const std::filesystem::path &path,
    const Objc3ImportedRuntimeModuleSurface &surface,
    std::string &error) {
  if (IsReadyObjc3ImportedRuntimeModuleSurfaceCrossModuleContract(surface)) {
    return true;
  }
  error = path.generic_string() +
          ": imported runtime module surface is not ready for cross-module consumption";
  return false;
}

bool PublishImportedRuntimeModulePackagingLinkPlanReadiness(
    const Objc3ImportedRuntimeModuleSurface &surface,
    const Objc3ImportedRuntimeModulePackagingPeerArtifacts &artifacts,
    std::string &error) {
  if (IsReadyObjc3ImportedRuntimeModulePackagingLinkPlan(artifacts)) {
    return true;
  }
  error = surface.source_path.generic_string() +
          ": imported runtime module packaging peer artifacts are not ready for link-plan consumption";
  return false;
}

}  // namespace objc3c::pipeline::runtime_import_preservation
