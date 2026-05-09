#include "driver/objc3_driver_frontend_error_bridge_artifact_publication.h"

#include "io/objc3_manifest_artifacts.h"

void PublishObjc3DriverFrontendErrorBridgeArtifact(
    const Objc3CliOptions &cli_options,
    const Objc3FrontendArtifactBundle &artifacts) {
  if (artifacts.error_handling_result_bridge_artifact_replay_json.empty()) {
    return;
  }
  WriteErrorHandlingResultBridgeArtifactReplay(
      cli_options.out_dir,
      cli_options.emit_prefix,
      artifacts.error_handling_result_bridge_artifact_replay_json);
}
