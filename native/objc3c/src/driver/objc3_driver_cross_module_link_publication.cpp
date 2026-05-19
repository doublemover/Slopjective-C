#include "driver/objc3_driver_cross_module_link_publication.h"

#include <cstddef>
#include <filesystem>
#include <string>

#include "driver/objc3_driver_cross_module_imported_surfaces.h"
#include "driver/objc3_driver_cross_module_imported_input.h"
#include "driver/objc3_driver_cross_module_link_plan_inputs.h"
#include "driver/objc3_driver_diagnostic_output.h"
#include "driver/objc3_driver_status_codes.h"
#include "io/objc3_manifest_artifacts.h"
#include "io/objc3_process.h"
#include "pipeline/objc3_runtime_import_surface.h"

int PublishObjc3DriverCrossModuleRuntimeLinkArtifacts(
    const Objc3CliOptions &cli_options,
    const Objc3FrontendArtifactBundle &artifacts,
    const Objc3RuntimeMetadataLinkerRetentionArtifacts
        &linker_retention_artifacts,
    const std::filesystem::path &object_out) {
  if (cli_options.imported_runtime_surface_paths.empty()) {
    return Objc3DriverStatusValue(Objc3DriverStatusCode::kSuccess);
  }

  Objc3DriverCrossModuleImportedSurfaces imported_surfaces;
  std::string imported_surfaces_error;
  if (!TryLoadObjc3DriverCrossModuleImportedSurfaces(
          cli_options.imported_runtime_surface_paths,
          imported_surfaces,
          imported_surfaces_error)) {
    EmitObjc3DriverError(imported_surfaces_error);
    return Objc3DriverStatusValue(
        Objc3DriverStatusCode::kHardCutoverContractFailure);
  }

  Objc3CrossModuleRuntimeLinkPlanArtifactInputs link_plan_inputs =
      BuildObjc3DriverCrossModuleRuntimeLinkPlanInputs(
          cli_options,
          artifacts,
          linker_retention_artifacts,
          object_out);
  for (std::size_t index = 0; index < imported_surfaces.surfaces.size();
       ++index) {
    AppendObjc3DriverCrossModuleRuntimeImportedInput(
        link_plan_inputs,
        imported_surfaces.surfaces[index],
        imported_surfaces.peer_artifacts[index]);
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
