#include "pipeline/runtime_import_link_plan.h"

#include "io/objc3_artifact_paths.h"

namespace objc3c::pipeline {

bool BuildObjc3ImportedRuntimeModuleLinkPlan(
    const std::filesystem::path &import_surface_path,
    Objc3ImportedRuntimeModuleLinkPlan &plan,
    std::string &error) {
  plan = Objc3ImportedRuntimeModuleLinkPlan{};
  plan.import_surface_path = import_surface_path;
  if (!TryResolveRuntimeImportEmitPrefixFromSurfacePath(
          import_surface_path, plan.emit_prefix, error)) {
    error = import_surface_path.generic_string() + ": " + error;
    return false;
  }

  plan.import_surface_parent_path = import_surface_path.parent_path();
  plan.registration_manifest_path =
      BuildRuntimeRegistrationManifestArtifactPath(
          plan.import_surface_parent_path, plan.emit_prefix)
          .lexically_normal();
  plan.discovery_artifact_path =
      BuildRuntimeMetadataDiscoveryArtifactPath(
          plan.import_surface_parent_path, plan.emit_prefix)
          .lexically_normal();
  plan.linker_response_artifact_path =
      BuildRuntimeMetadataLinkerResponseArtifactPath(
          plan.import_surface_parent_path, plan.emit_prefix)
          .lexically_normal();
  return true;
}

}  // namespace objc3c::pipeline
