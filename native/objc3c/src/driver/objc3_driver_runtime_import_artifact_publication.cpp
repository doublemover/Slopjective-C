#include "driver/objc3_driver_runtime_import_artifact_publication.h"

#include "driver/objc3_driver_diagnostic_output.h"
#include "driver/objc3_driver_runtime_import_artifact_readiness.h"
#include "driver/objc3_driver_status_codes.h"
#include "io/objc3_manifest_artifacts.h"

Objc3DriverRuntimeImportArtifactPublicationResult
PublishObjc3DriverRuntimeImportArtifacts(
    const Objc3CliOptions &cli_options,
    const Objc3FrontendArtifactBundle &artifacts) {
  const Objc3DriverRuntimeImportArtifactReadiness readiness =
      CheckObjc3DriverRuntimeImportArtifactReadiness(artifacts);
  if (!readiness.ready) {
    EmitObjc3DriverError(
        "runtime-aware import/module frontend closure not ready");
    return {.status_code = Objc3DriverStatusValue(
                Objc3DriverStatusCode::kHardCutoverContractFailure),
            .has_runtime_import_artifact =
                readiness.has_runtime_import_artifact};
  }
  if (readiness.has_runtime_import_artifact) {
    WriteRuntimeAwareImportModuleArtifact(
        cli_options.out_dir,
        cli_options.emit_prefix,
        artifacts.runtime_aware_import_module_artifact_json);
  }
  return {.status_code = Objc3DriverStatusValue(
              Objc3DriverStatusCode::kSuccess),
          .has_runtime_import_artifact =
              readiness.has_runtime_import_artifact};
}
