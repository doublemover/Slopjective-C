#include "driver/objc3_driver_frontend_runtime_metadata_artifact_publication.h"

#include "io/objc3_manifest_artifacts.h"

void PublishObjc3DriverFrontendRuntimeMetadataArtifact(
    const Objc3CliOptions &cli_options,
    const Objc3FrontendArtifactBundle &artifacts) {
  if (artifacts.runtime_metadata_binary.empty()) {
    return;
  }
  WriteRuntimeMetadataBinaryArtifact(cli_options.out_dir,
                                     cli_options.emit_prefix,
                                     artifacts.runtime_metadata_binary);
}
