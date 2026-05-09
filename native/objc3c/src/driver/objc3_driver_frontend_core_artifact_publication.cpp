#include "driver/objc3_driver_frontend_core_artifact_publication.h"

#include "io/objc3_diagnostics_artifacts.h"
#include "io/objc3_manifest_artifacts.h"

Objc3DriverFrontendCoreArtifactPublicationResult
PublishObjc3DriverFrontendCoreArtifacts(
    const Objc3CliOptions &cli_options,
    const Objc3FrontendArtifactBundle &artifacts) {
  WriteDiagnosticsArtifacts(cli_options.out_dir,
                            cli_options.emit_prefix,
                            artifacts.stage_diagnostics,
                            artifacts.post_pipeline_diagnostics);
  if (!artifacts.manifest_json.empty()) {
    WriteManifestArtifact(cli_options.out_dir,
                          cli_options.emit_prefix,
                          artifacts.manifest_json);
  }
  if (!artifacts.runtime_metadata_binary.empty()) {
    WriteRuntimeMetadataBinaryArtifact(cli_options.out_dir,
                                       cli_options.emit_prefix,
                                       artifacts.runtime_metadata_binary);
  }
  if (!artifacts.error_handling_result_bridge_artifact_replay_json.empty()) {
    WriteErrorHandlingResultBridgeArtifactReplay(
        cli_options.out_dir,
        cli_options.emit_prefix,
        artifacts.error_handling_result_bridge_artifact_replay_json);
  }
  return {.diagnostics_present = !artifacts.diagnostics.empty()};
}
