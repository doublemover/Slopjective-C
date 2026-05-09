#include "driver/objc3_driver_runtime_import_artifact_publication.h"

#include <iostream>

#include "io/objc3_manifest_artifacts.h"
#include "pipeline/objc3_runtime_import_surface.h"

Objc3DriverRuntimeImportArtifactPublicationResult
PublishObjc3DriverRuntimeImportArtifacts(
    const Objc3CliOptions &cli_options,
    const Objc3FrontendArtifactBundle &artifacts) {
  const bool has_runtime_import_artifact =
      !artifacts.runtime_aware_import_module_artifact_json.empty();
  if (has_runtime_import_artifact &&
      !IsReadyObjc3RuntimeAwareImportModuleFrontendClosureSummary(
          artifacts.runtime_aware_import_module_frontend_closure_summary)) {
    std::cerr << "runtime-aware import/module frontend closure not ready\n";
    return {.status_code = 125,
            .has_runtime_import_artifact = has_runtime_import_artifact};
  }
  if (has_runtime_import_artifact) {
    WriteRuntimeAwareImportModuleArtifact(
        cli_options.out_dir,
        cli_options.emit_prefix,
        artifacts.runtime_aware_import_module_artifact_json);
  }
  return {.status_code = 0,
          .has_runtime_import_artifact = has_runtime_import_artifact};
}
