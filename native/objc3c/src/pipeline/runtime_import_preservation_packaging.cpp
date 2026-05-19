#include "pipeline/runtime_import_preservation_owners.h"

#include <filesystem>
#include <string>
#include <utility>
#include <vector>

#include "pipeline/runtime_import_link_plan.h"
#include "pipeline/runtime_import_packaging_peer_artifacts.h"
#include "support/objc3_file_reading.h"

namespace objc3c::pipeline::runtime_import_preservation {

bool LoadImportedRuntimeModulePackagingPeerArtifacts(
    const Objc3ImportedRuntimeModuleSurface &surface,
    Objc3ImportedRuntimeModulePackagingPeerArtifacts &artifacts,
    std::string &error) {
  artifacts = Objc3ImportedRuntimeModulePackagingPeerArtifacts{};
  error.clear();

  Objc3ImportedRuntimeModuleLinkPlan link_plan;
  if (!BuildObjc3ImportedRuntimeModuleLinkPlan(surface.source_path, link_plan,
                                               error)) {
    return false;
  }

  std::string manifest_io_error;
  std::string manifest_payload;
  if (!objc3c::support::TryReadTextFile(link_plan.registration_manifest_path,
                                        manifest_payload,
                                        manifest_io_error,
                                        "unable to open file",
                                        "failed to read file")) {
    error = link_plan.registration_manifest_path.generic_string() + ": " +
            manifest_io_error;
    return false;
  }
  RuntimeImportJsonParser manifest_parser(manifest_payload);
  RuntimeImportJsonValue manifest_root_value;
  std::string manifest_parse_error;
  if (!manifest_parser.Parse(manifest_root_value, manifest_parse_error)) {
    error = link_plan.registration_manifest_path.generic_string() + ": " +
            manifest_parse_error;
    return false;
  }
  const RuntimeImportJsonValue::Object *manifest_root_object =
      AsObject(manifest_root_value);
  if (manifest_root_object == nullptr) {
    error = link_plan.registration_manifest_path.generic_string() +
            ": runtime registration manifest payload must be a JSON object";
    return false;
  }

  Objc3ImportedRuntimeModulePackagingPeerArtifacts parsed_artifacts;
  if (!PopulateImportedRuntimeRegistrationManifestPeerArtifacts(
          *manifest_root_object, parsed_artifacts, manifest_parse_error)) {
    error = link_plan.registration_manifest_path.generic_string() + ": " +
            manifest_parse_error;
    return false;
  }

  std::string discovery_io_error;
  std::string discovery_payload;
  if (!objc3c::support::TryReadTextFile(link_plan.discovery_artifact_path,
                                        discovery_payload,
                                        discovery_io_error,
                                        "unable to open file",
                                        "failed to read file")) {
    error = link_plan.discovery_artifact_path.generic_string() + ": " +
            discovery_io_error;
    return false;
  }
  RuntimeImportJsonParser discovery_parser(discovery_payload);
  RuntimeImportJsonValue discovery_root_value;
  std::string discovery_parse_error;
  if (!discovery_parser.Parse(discovery_root_value, discovery_parse_error)) {
    error = link_plan.discovery_artifact_path.generic_string() + ": " +
            discovery_parse_error;
    return false;
  }
  const RuntimeImportJsonValue::Object *discovery_root_object =
      AsObject(discovery_root_value);
  if (discovery_root_object == nullptr) {
    error = link_plan.discovery_artifact_path.generic_string() +
            ": runtime metadata discovery payload must be a JSON object";
    return false;
  }

  std::string object_artifact_relative_path;
  if (!ValidateImportedRuntimeDiscoveryPeerArtifacts(*discovery_root_object,
                                                     parsed_artifacts,
                                                     object_artifact_relative_path,
                                                     discovery_parse_error)) {
    error = link_plan.discovery_artifact_path.generic_string() + ": " +
            discovery_parse_error;
    return false;
  }

  std::string response_io_error;
  std::string linker_response_payload;
  if (!objc3c::support::TryReadTextFile(link_plan.linker_response_artifact_path,
                                        linker_response_payload,
                                        response_io_error,
                                        "unable to open file",
                                        "failed to read file")) {
    error = link_plan.linker_response_artifact_path.generic_string() + ": " +
            response_io_error;
    return false;
  }
  const std::vector<std::string> response_flags =
      SplitRuntimeImportLinkerResponseFlags(linker_response_payload);
  if (response_flags != parsed_artifacts.driver_linker_flags) {
    error = link_plan.linker_response_artifact_path.generic_string() +
            ": linker response payload drifted from imported driver linker flags";
    return false;
  }

  const std::filesystem::path object_artifact_path =
      (link_plan.import_surface_parent_path / object_artifact_relative_path)
          .lexically_normal();
  if (!std::filesystem::exists(object_artifact_path)) {
    error = object_artifact_path.generic_string() +
            ": imported object artifact is missing";
    return false;
  }

  parsed_artifacts.registration_manifest_path =
      link_plan.registration_manifest_path;
  parsed_artifacts.discovery_artifact_path = link_plan.discovery_artifact_path;
  parsed_artifacts.linker_response_artifact_path =
      link_plan.linker_response_artifact_path;
  parsed_artifacts.object_artifact_path = object_artifact_path;
  if (!PublishImportedRuntimeModulePackagingLinkPlanReadiness(
          surface, parsed_artifacts, error)) {
    return false;
  }
  artifacts = std::move(parsed_artifacts);
  return true;
}

}  // namespace objc3c::pipeline::runtime_import_preservation
