#include "driver/objc3_driver_frontend_manifest_artifact_publication.h"

#include "io/objc3_manifest_artifacts.h"

void PublishObjc3DriverFrontendManifestArtifact(
    const Objc3CliOptions &cli_options,
    const Objc3FrontendArtifactBundle &artifacts) {
  if (artifacts.manifest_json.empty()) {
    return;
  }
  WriteManifestArtifact(cli_options.out_dir,
                        cli_options.emit_prefix,
                        artifacts.manifest_json);
}
