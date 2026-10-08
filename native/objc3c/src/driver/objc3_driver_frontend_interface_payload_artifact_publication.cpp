#include "driver/objc3_driver_frontend_interface_payload_artifact_publication.h"

#include "io/objc3_artifact_writers.h"

void PublishObjc3DriverFrontendInterfacePayloadArtifact(
    const Objc3CliOptions &cli_options,
    const Objc3FrontendArtifactBundle &artifacts) {
  if (artifacts.standalone_textual_interface_payload_json.empty()) {
    return;
  }
  WriteStandaloneTextualInterfacePayloadArtifact(
      cli_options.out_dir,
      cli_options.emit_prefix,
      artifacts.standalone_textual_interface_payload_json);
}
