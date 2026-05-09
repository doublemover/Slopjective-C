#include "driver/objc3_driver_cross_module_link_publication.h"

#include <cstddef>
#include <filesystem>
#include <string>
#include <utility>
#include <vector>

#include "driver/objc3_driver_cross_module_imported_input.h"
#include "driver/objc3_driver_cross_module_link_plan_inputs.h"
#include "driver/objc3_driver_diagnostic_output.h"
#include "driver/objc3_driver_status_codes.h"
#include "io/objc3_manifest_artifacts.h"
#include "io/objc3_process.h"
#include "pipeline/objc3_runtime_import_surface.h"

namespace fs = std::filesystem;

int PublishObjc3DriverCrossModuleRuntimeLinkArtifacts(
    const Objc3CliOptions &cli_options,
    const Objc3FrontendArtifactBundle &artifacts,
    const Objc3RuntimeMetadataLinkerRetentionArtifacts
        &linker_retention_artifacts,
    const std::filesystem::path &object_out) {
  if (cli_options.imported_runtime_surface_paths.empty()) {
    return Objc3DriverStatusValue(Objc3DriverStatusCode::kSuccess);
  }

  std::vector<Objc3ImportedRuntimeModuleSurface> imported_surfaces;
  imported_surfaces.reserve(cli_options.imported_runtime_surface_paths.size());
  std::vector<Objc3ImportedRuntimeModulePackagingPeerArtifacts>
      imported_peer_artifacts;
  imported_peer_artifacts.reserve(
      cli_options.imported_runtime_surface_paths.size());

  for (const auto &import_path : cli_options.imported_runtime_surface_paths) {
    Objc3ImportedRuntimeModuleSurface imported_surface;
    std::string import_surface_error;
    const fs::path absolute_import_path =
        fs::absolute(import_path).lexically_normal();
    if (!TryLoadObjc3ImportedRuntimeModuleSurface(
            absolute_import_path, imported_surface, import_surface_error)) {
      EmitObjc3DriverError(import_surface_error);
      return Objc3DriverStatusValue(
          Objc3DriverStatusCode::kHardCutoverContractFailure);
    }

    Objc3ImportedRuntimeModulePackagingPeerArtifacts peer_artifacts;
    std::string peer_artifacts_error;
    if (!TryLoadObjc3ImportedRuntimeModulePackagingPeerArtifacts(
            imported_surface, peer_artifacts, peer_artifacts_error)) {
      EmitObjc3DriverError(peer_artifacts_error);
      return Objc3DriverStatusValue(
          Objc3DriverStatusCode::kHardCutoverContractFailure);
    }
    imported_surfaces.push_back(std::move(imported_surface));
    imported_peer_artifacts.push_back(std::move(peer_artifacts));
  }

  Objc3CrossModuleRuntimeLinkPlanArtifactInputs link_plan_inputs =
      BuildObjc3DriverCrossModuleRuntimeLinkPlanInputs(
          cli_options,
          artifacts,
          linker_retention_artifacts,
          object_out);
  for (std::size_t index = 0; index < imported_surfaces.size(); ++index) {
    AppendObjc3DriverCrossModuleRuntimeImportedInput(
        link_plan_inputs,
        imported_surfaces[index],
        imported_peer_artifacts[index]);
  }

  std::string link_plan_json;
  std::string cross_module_linker_response_payload;
  std::string link_plan_error;
  if (!TryBuildObjc3CrossModuleRuntimeLinkPlanArtifact(
          link_plan_inputs,
          link_plan_json,
          cross_module_linker_response_payload,
          link_plan_error)) {
    EmitObjc3DriverError(link_plan_error);
    return Objc3DriverStatusValue(
        Objc3DriverStatusCode::kHardCutoverContractFailure);
  }

  WriteCrossModuleRuntimeLinkPlanArtifact(
      cli_options.out_dir,
      cli_options.emit_prefix,
      link_plan_json);
  WriteCrossModuleRuntimeLinkerResponseArtifact(
      cli_options.out_dir,
      cli_options.emit_prefix,
      cross_module_linker_response_payload);
  return Objc3DriverStatusValue(Objc3DriverStatusCode::kSuccess);
}
