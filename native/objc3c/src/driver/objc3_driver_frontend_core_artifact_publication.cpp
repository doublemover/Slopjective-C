#include "driver/objc3_driver_frontend_core_artifact_publication.h"

#include "driver/objc3_driver_frontend_diagnostic_artifact_publication.h"
#include "driver/objc3_driver_frontend_error_bridge_artifact_publication.h"
#include "driver/objc3_driver_frontend_manifest_artifact_publication.h"
#include "driver/objc3_driver_frontend_runtime_metadata_artifact_publication.h"

Objc3DriverFrontendCoreArtifactPublicationResult
PublishObjc3DriverFrontendCoreArtifacts(
    const Objc3CliOptions &cli_options,
    const Objc3FrontendArtifactBundle &artifacts) {
  const bool diagnostics_present =
      PublishObjc3DriverFrontendDiagnosticArtifacts(cli_options, artifacts);
  PublishObjc3DriverFrontendManifestArtifact(cli_options, artifacts);
  PublishObjc3DriverFrontendRuntimeMetadataArtifact(cli_options, artifacts);
  PublishObjc3DriverFrontendErrorBridgeArtifact(cli_options, artifacts);
  return {.diagnostics_present = diagnostics_present};
}
