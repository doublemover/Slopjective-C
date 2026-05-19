#include "driver/objc3_driver_interop_artifact_publication.h"

#include "io/objc3_manifest_artifacts.h"

void PublishObjc3DriverInteropArtifacts(
    const Objc3CliOptions &cli_options,
    const Objc3FrontendArtifactBundle &artifacts) {
  if (!artifacts.interop_bridge_header_artifact_text.empty()) {
    WriteInteropBridgeHeaderArtifact(
        cli_options.out_dir,
        cli_options.emit_prefix,
        artifacts.interop_bridge_header_artifact_text);
  }
  if (!artifacts.interop_bridge_module_artifact_text.empty()) {
    WriteInteropBridgeModuleArtifact(
        cli_options.out_dir,
        cli_options.emit_prefix,
        artifacts.interop_bridge_module_artifact_text);
  }
  if (!artifacts.interop_bridge_artifact_json.empty()) {
    WriteInteropBridgeArtifact(cli_options.out_dir,
                               cli_options.emit_prefix,
                               artifacts.interop_bridge_artifact_json);
  }
}
