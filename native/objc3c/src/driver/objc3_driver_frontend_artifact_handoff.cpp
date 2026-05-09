#include "driver/objc3_driver_frontend_artifact_handoff.h"

#include "driver/objc3_driver_frontend_core_artifact_publication.h"
#include "driver/objc3_driver_interop_artifact_publication.h"
#include "driver/objc3_driver_runtime_import_artifact_publication.h"
#include "driver/objc3_driver_status_codes.h"

Objc3DriverFrontendArtifactHandoffResult
PublishObjc3DriverFrontendArtifactHandoff(
    const Objc3CliOptions &cli_options,
    const Objc3FrontendArtifactBundle &artifacts) {
  const Objc3DriverFrontendCoreArtifactPublicationResult core_publication =
      PublishObjc3DriverFrontendCoreArtifacts(cli_options, artifacts);
  if (core_publication.diagnostics_present) {
    return {.status_code = Objc3DriverStatusValue(
                Objc3DriverStatusCode::kDiagnosticsPresent),
            .has_runtime_import_artifact = false};
  }

  const Objc3DriverRuntimeImportArtifactPublicationResult runtime_import =
      PublishObjc3DriverRuntimeImportArtifacts(cli_options, artifacts);
  if (runtime_import.status_code != 0) {
    return {.status_code = runtime_import.status_code,
            .has_runtime_import_artifact =
                runtime_import.has_runtime_import_artifact};
  }
  PublishObjc3DriverInteropArtifacts(cli_options, artifacts);
  return {.status_code = Objc3DriverStatusValue(
              Objc3DriverStatusCode::kSuccess),
          .has_runtime_import_artifact =
              runtime_import.has_runtime_import_artifact};
}
